#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
N="${1:-50000}"
NLISTAS="${2:-8}"
REPS="${3:-3}"
OUT="results.csv"
MAX_THREADS=$(nproc)
printf 'threads,v1_critical_nomeado_s,v2_locks_s\n' > "$OUT"
gcc -std=c11 -O2 -Wall -fopenmp v1_critical_nomeado.c -o v1_critical -lm
gcc -std=c11 -O2 -Wall -fopenmp v2_locks.c -o v2_locks -lm
median_of() {
  python3 -c "import statistics,sys; print(f'{statistics.median(float(x) for x in sys.argv[1:]):.4f}')" "$@"
}
for ((t=1; t<=MAX_THREADS; t++)); do
  export OMP_NUM_THREADS=$t
  a_vals=(); b_vals=()
  for ((r=1; r<=REPS; r++)); do
    a_vals+=("$(./v1_critical "$N" | grep -oP 'tempo=\K[0-9.]+')")
    b_vals+=("$(./v2_locks "$N" "$NLISTAS" | grep -oP 'tempo=\K[0-9.]+')")
  done
  t1=$(median_of "${a_vals[@]}")
  t2=$(median_of "${b_vals[@]}")
  printf '%d,%s,%s\n' "$t" "$t1" "$t2" >> "$OUT"
  echo "threads=$t v1=$t1 v2=$t2"
done
echo "Gerando graficos..."
python3 plot_results.py
cat "$OUT"
