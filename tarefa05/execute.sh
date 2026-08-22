#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-5000000}"
OUT="results.csv"
MAX_THREADS=$(nproc)

printf 'threads,seq_time,omp_naive_time,omp_red_time,seq_count,omp_naive_count,omp_red_count\n' > "$OUT"

gcc -std=c11 -Wall -Wextra -O3 -fopenmp tarefa05.c -o tarefa05 -lm

echo "Iniciando experimento com N=$N e max_threads=$MAX_THREADS"

for ((t=1; t<=MAX_THREADS; t++)); do
    export OMP_NUM_THREADS=$t
    linha=$(./tarefa05 "$N")
    valores=$(printf '%s\n' "$linha" | sed -E 's/.*seq_time=([^ ]+) omp_naive_time=([^ ]+) omp_red_time=([^ ]+) seq_count=([^ ]+) omp_naive_count=([^ ]+) omp_red_count=([^ ]+).*/\1,\2,\3,\4,\5,\6/')
    printf '%d,%s\n' "$t" "$valores" >> "$OUT"
    echo "Concluído threads=$t"
done

rm -f tarefa05

echo "Gerando gráficos..."
python3 plot_results.py

cat "$OUT"
