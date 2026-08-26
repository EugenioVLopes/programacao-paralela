#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-50000000}"
OUT="results.csv"
MAX_THREADS=$(nproc)

printf 'threads,memory_bound,cpu_bound\n' > "$OUT"

gcc -std=c11 -Wall -Wextra -fopenmp tarefa04.c -o tarefa04 -lm

echo "Iniciando experimento com N=$N e max_threads=$MAX_THREADS"

for ((t=1; t<=MAX_THREADS; t++)); do
    export OMP_NUM_THREADS=$t
    export OMP_DYNAMIC=false
    valores=$(./tarefa04 "$N" | sed -E 's/.*memory_bound=([^ ]+) cpu_bound=([^ ]+).*/\1,\2/')
    printf '%d,%s\n' "$t" "$valores" >> "$OUT"
    echo "Concluído threads=$t"
done

rm -f tarefa04

echo "Gerando gráficos..."
python3 plot_results.py

cat "$OUT"
