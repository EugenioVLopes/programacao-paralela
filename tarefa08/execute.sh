#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
N="${1:-5000000}"
REPS="${2:-5}"
OUT="results.csv"
MAX_THREADS=$(nproc)
printf 'threads,v1_critical_rand_s,v2_vector_rand_s,v3_critical_randr_s,v4_vector_randr_s,v4pad_vector_randr_pad_s\n' > "$OUT"
gcc -std=c11 -O2 -Wall -fopenmp v1_pi_critical_rand.c -o pi_critical_rand -lm
gcc -std=c11 -O2 -Wall -fopenmp v2_pi_vector_rand.c -o pi_vector_rand -lm
gcc -std=c11 -O2 -Wall -fopenmp v3_pi_critical_randr.c -o pi_critical_randr -lm
gcc -std=c11 -O2 -Wall -fopenmp v4_pi_vector_randr.c -o pi_vector_randr -lm
gcc -std=c11 -O2 -Wall -fopenmp v4_fix_pi_vector_randr_pad.c -o pi_vector_randr_pad -lm
median_of() {
  python3 -c "import statistics,sys; print(f'{statistics.median(float(x) for x in sys.argv[1:]):.4f}')" "$@"
}
for ((t=1; t<=MAX_THREADS; t++)); do
  export OMP_NUM_THREADS=$t
  a_vals=(); b_vals=(); c_vals=(); d_vals=(); e_vals=()
  for ((r=1; r<=REPS; r++)); do
    a_vals+=("$(./pi_critical_rand "$N" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')")
    b_vals+=("$(./pi_vector_rand "$N" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')")
    c_vals+=("$(./pi_critical_randr "$N" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')")
    d_vals+=("$(./pi_vector_randr "$N" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')")
    e_vals+=("$(./pi_vector_randr_pad "$N" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')")
  done
  t1=$(median_of "${a_vals[@]}")
  t2=$(median_of "${b_vals[@]}")
  t3=$(median_of "${c_vals[@]}")
  t4=$(median_of "${d_vals[@]}")
  t5=$(median_of "${e_vals[@]}")
  printf '%d,%s,%s,%s,%s,%s\n' "$t" "$t1" "$t2" "$t3" "$t4" "$t5" >> "$OUT"
  echo "threads=$t v1=$t1 v2=$t2 v3=$t3 v4=$t4 v4pad=$t5"
done
echo "Gerando graficos..."
python3 plot_results.py
cat "$OUT"
