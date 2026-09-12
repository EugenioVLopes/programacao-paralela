#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

REPS=${REPS:-10}
SLEEP_SECONDS=${SLEEP_SECONDS:-1}
RANDOM_SEED=${RANDOM_SEED:-3703}
MAX_THREADS=$(nproc)
OUT=${OUT:-"runs/stable-$(date +%Y%m%d-%H%M%S)-$$"}
CC=${CC:-gcc}

[[ $REPS =~ ^[1-9][0-9]*$ ]] || { echo "REPS deve ser inteiro positivo" >&2; exit 1; }
[[ $RANDOM_SEED =~ ^[0-9]+$ ]] || { echo "RANDOM_SEED deve ser inteiro não negativo" >&2; exit 1; }
[[ $SLEEP_SECONDS =~ ^([0-9]+([.][0-9]*)?|[.][0-9]+)$ ]] || {
  echo "SLEEP_SECONDS deve ser um número não negativo" >&2
  exit 1
}

mkdir -p "$(dirname "$OUT")"
mkdir "$OUT" # Não sobrescrever resultados anteriores.
BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=${OMP_PROC_BIND:-close}
export OMP_PLACES=${OMP_PLACES:-cores}
export OMP_WAIT_POLICY=${OMP_WAIT_POLICY:-ACTIVE}

for source in v*.c; do
  flags=()
  [[ $source == v0* ]] || flags=(-fopenmp)
  "$CC" -std=c11 -O2 -Wall -Wextra "${flags[@]}" \
    "$source" -lm -o "$BUILD/${source%.c}"
done

python3 - "$OUT/order.csv" "$REPS" "$MAX_THREADS" "$RANDOM_SEED" <<'PY'
import csv
import random
import sys

output, repetitions, max_threads, seed = sys.argv[1:]
repetitions, max_threads, seed = map(int, (repetitions, max_threads, seed))
parallel_versions = [
    "v1_static",
    "v2_static_collapse",
    "v3_dynamic",
    "v4_dynamic_collapse",
    "v5_guided_collapse",
    "v6_guided",
]
configurations = [("v0_seq", 1)] + [
    (version, threads)
    for version in parallel_versions
    for threads in range(1, max_threads + 1)
]
rng = random.Random(seed)
with open(output, "w", newline="") as file:
    writer = csv.writer(file, lineterminator="\n")
    writer.writerow(("rep", "order", "version", "threads"))
    for repetition in range(1, repetitions + 1):
        current = configurations.copy()
        rng.shuffle(current)
        for order, (version, threads) in enumerate(current, start=1):
            writer.writerow((repetition, order, version, threads))
PY

# Aquecimento de todas as configurações antes das medições.
for binary in "$BUILD"/v*; do
  counts=$(seq 1 "$MAX_THREADS")
  [[ $(basename "$binary") == v0* ]] && counts=1
  for threads in $counts; do
    OMP_NUM_THREADS=$threads "$binary" >/dev/null
  done
done
sleep 2

printf 'version,threads,rep,seconds\n' > "$OUT/raw.csv"
tail -n +2 "$OUT/order.csv" | while IFS=, read -r rep order version threads; do
  sleep "$SLEEP_SECONDS"
  line=$(OMP_NUM_THREADS=$threads "$BUILD/$version")
  seconds=${line##* tempo=}; seconds=${seconds%% *}
  actual=${line#*threads=}; actual=${actual%% *}
  [[ $actual == "$threads" ]] || {
    echo "Threads reais ($actual) diferem das solicitadas ($threads)" >&2
    exit 1
  }
  printf '%s,%s,%s,%s\n' "$version" "$threads" "$rep" "$seconds" >> "$OUT/raw.csv"
  printf 'repetição=%d/%d ordem=%d versão=%s threads=%d tempo=%ss\n' \
    "$rep" "$REPS" "$order" "$version" "$threads" "$seconds"
done

{
  date -Iseconds
  uname -a
  "$CC" --version
  echo "flags=-std=c11 -O2 -Wall -Wextra; paralelas=-fopenmp; link=-lm"
  echo "tamanho_grid=512 num_passos_tempo=500"
  echo "repeticoes=$REPS sleep_seconds=$SLEEP_SECONDS random_seed=$RANDOM_SEED"
  echo "threads=1..$MAX_THREADS"
  echo "OMP_DYNAMIC=$OMP_DYNAMIC OMP_PROC_BIND=$OMP_PROC_BIND OMP_PLACES=$OMP_PLACES OMP_WAIT_POLICY=$OMP_WAIT_POLICY"
  lscpu
  sha256sum v*.c
} > "$OUT/metadata.txt"

echo
echo "Benchmark concluído: $OUT"
echo "Gerar gráficos: python3 plot_results.py '$OUT/raw.csv'"
