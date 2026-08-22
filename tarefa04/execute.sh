#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-50000000}"
OUT="results.csv"
MAX_THREADS=$(nproc)

printf 'threads,memory_bound,cpu_bound\n' > "$OUT"

# Compilar com OpenMP e otimização agressiva
gcc -std=c11 -Wall -Wextra -O3 -fopenmp tarefa04.c -o tarefa04 -lm

echo "Iniciando experimento com N=$N e max_threads=$MAX_THREADS"

# Executar variando OMP_NUM_THREADS
for ((t=1; t<=MAX_THREADS; t++)); do
    export OMP_NUM_THREADS=$t
    linha=$(./tarefa04 "$N")
    valores=$(printf '%s\n' "$linha" | sed -E 's/.*memory_bound=([^ ]+) cpu_bound=([^ ]+).*/\1,\2/')
    printf '%d,%s\n' "$t" "$valores" >> "$OUT"
    echo "Concluído threads=$t"
done

# Limpar o binário
rm -f tarefa04

echo "Gerando gráficos..."
python3 plot_results.py

cat "$OUT"
