#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

REPS=5
THREADS=8
OUT="runs/chunks-$(date +%Y%m%d-%H%M%S)-$$"
mkdir -p "$(dirname "$OUT")"
mkdir "$OUT"
CC=${CC:-gcc}
export OMP_NUM_THREADS=$THREADS
export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=${OMP_PROC_BIND:-close}
export OMP_PLACES=${OMP_PLACES:-cores}

printf 'schedule,unit,chunk,equivalent_rows,rep,seconds\n' > "$OUT/raw_chunks.csv"

measure() {
 local source=$1 schedule=$2 unit=$3 chunk=$4 equivalent_rows=$5 binary="$OUT/program"
 "$CC" -std=c11 -O2 -Wall -Wextra -fopenmp -DCHUNK_SIZE="$chunk" \
   "$source" -lm -o "$binary"
 "$binary" >/dev/null
 for ((rep=1; rep<=REPS; rep++)); do
  line=$("$binary")
  seconds=${line##* tempo=}; seconds=${seconds%% *}
  printf '%s,%s,%s,%s,%s,%s\n' \
    "$schedule" "$unit" "$chunk" "$equivalent_rows" "$rep" "$seconds" \
    >> "$OUT/raw_chunks.csv"
 done
}

# Sem collapse, o chunk conta linhas. Com collapse, conta células.
# Como há 510 células internas por linha, os pares abaixo têm volume equivalente.
for rows in 1 4 16 64; do
 cells=$((rows * 510))
 measure v3_dynamic.c dynamic linhas "$rows" "$rows"
 measure v4_dynamic_collapse.c dynamic celulas "$cells" "$rows"
 measure v6_guided.c guided linhas "$rows" "$rows"
 measure v5_guided_collapse.c guided celulas "$cells" "$rows"
done

rm -f "$OUT/program"
{
 date -Iseconds
 uname -a
 "$CC" --version
 echo "tamanho_grid=512 num_passos_tempo=500 threads=$THREADS repeticoes=$REPS"
 echo "chunks equivalentes: linhas={1,4,16,64}; celulas=linhas*510"
 echo "OMP_DYNAMIC=$OMP_DYNAMIC OMP_PROC_BIND=$OMP_PROC_BIND OMP_PLACES=$OMP_PLACES"
 sha256sum v3_dynamic.c v4_dynamic_collapse.c v5_guided_collapse.c v6_guided.c
} > "$OUT/metadata.txt"

echo "Experimento de chunks: $OUT"
echo "Gráfico: python3 plot_chunks.py '$OUT/raw_chunks.csv'"
