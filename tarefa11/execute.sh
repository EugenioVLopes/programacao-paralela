#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
REPS=5
MAX_THREADS=$(nproc)
THREADS=$(seq 1 "$MAX_THREADS")
OUT=${OUT:-"runs/$(date +%Y%m%d-%H%M%S)-$$"}
mkdir -p "$(dirname "$OUT")"
mkdir "$OUT" # Não sobrescrever uma execução existente.
CC=${CC:-gcc}
export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=${OMP_PROC_BIND:-close}
export OMP_PLACES=${OMP_PLACES:-cores}
{
 date -Iseconds
 uname -a
 "$CC" --version
 echo "flags=-std=c11 -O2 -Wall -Wextra; paralelas=-fopenmp; link=-lm"
 echo "tamanho_grid=512 num_passos_tempo=500 REPS=$REPS THREADS=1..$MAX_THREADS"
 echo "OMP_DYNAMIC=$OMP_DYNAMIC OMP_PROC_BIND=$OMP_PROC_BIND OMP_PLACES=$OMP_PLACES"
 lscpu
 sha256sum v*.c
} > "$OUT/metadata.txt"
for source in v*.c; do
 flags=(); [[ $source == v0* ]] || flags=(-fopenmp)
 "$CC" -std=c11 -O2 -Wall -Wextra "${flags[@]}" "$source" -lm -o "$OUT/${source%.c}"
done
printf 'version,threads,rep,seconds\n' > "$OUT/raw.csv"
for binary in "$OUT"/v*; do
 counts=$THREADS; [[ $(basename "$binary") == v0* ]] && counts=1
 for t in $counts; do
  export OMP_NUM_THREADS=$t
  "$binary" > /dev/null # aquecimento
  for ((rep=1;rep<=REPS;rep++)); do
   line=$("$binary")
   echo "$line" >> "$OUT/stdout.txt"
   seconds=${line##* tempo=}; seconds=${seconds%% *}
   actual=${line#*threads=}; actual=${actual%% *}
   [[ $actual == "$t" ]] || { echo "Threads reais ($actual) diferem das solicitadas ($t)" >&2; exit 1; }
   printf '%s,%s,%s,%s\n' "$(basename "$binary")" "$t" "$rep" "$seconds" >> "$OUT/raw.csv"
  done
 done
done
rm -f "$OUT"/v0_seq "$OUT"/v1_static "$OUT"/v2_static_collapse \
      "$OUT"/v3_dynamic "$OUT"/v4_dynamic_collapse \
      "$OUT"/v5_guided_collapse "$OUT"/v6_guided
echo "Dados brutos e metadados: $OUT"
echo "Gráficos opcionais: python3 plot_results.py '$OUT/raw.csv'"
