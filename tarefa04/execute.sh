#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-5000000}"
OUT="results.csv"
MAX_THREADS=$(nproc)
NUM_RUNS=10

gcc -std=c11 -Wall -Wextra -fopenmp tarefa04.c -o tarefa04 -lm

echo "Iniciando experimento com N=$N, max_threads=$MAX_THREADS, NUM_RUNS=$NUM_RUNS"

# Armazenar resultados temporários em arquivos separados por thread
for ((t=1; t<=MAX_THREADS; t++)); do
    mem_file=$(mktemp)
    cpu_file=$(mktemp)
    echo "$mem_file" >> /tmp/.tarefa04_mem_files
    echo "$cpu_file" >> /tmp/.tarefa04_cpu_files
done

rm -f /tmp/.tarefa04_mem_files /tmp/.tarefa04_cpu_files
for ((t=1; t<=MAX_THREADS; t++)); do
    : > "/tmp/.tarefa04_mem_$t"
    : > "/tmp/.tarefa04_cpu_$t"
done

export OMP_DYNAMIC=false

for ((run=1; run<=NUM_RUNS; run++)); do
    echo "Execução $run/$NUM_RUNS"
    for ((t=1; t<=MAX_THREADS; t++)); do
        export OMP_NUM_THREADS=$t
        valores=$(./tarefa04 "$N" | sed -E 's/.*memory_bound=([^ ]+) cpu_bound=([^ ]+).*/\1,\2/')
        mem_val=$(echo "$valores" | cut -d',' -f1)
        cpu_val=$(echo "$valores" | cut -d',' -f2)
        echo "$mem_val" >> "/tmp/.tarefa04_mem_$t"
        echo "$cpu_val" >> "/tmp/.tarefa04_cpu_$t"
    done
done

rm -f tarefa04

echo "Calculando médias e desvios padrão..."

printf 'threads,memory_bound_mean,memory_bound_std,cpu_bound_mean,cpu_bound_std\n' > "$OUT"

for ((t=1; t<=MAX_THREADS; t++)); do
    python3 -c "
import sys
mem = [float(x) for x in open('/tmp/.tarefa04_mem_$t').read().split() if x.strip()]
cpu = [float(x) for x in open('/tmp/.tarefa04_cpu_$t').read().split() if x.strip()]
import statistics
mem_mean = statistics.mean(mem)
mem_std = statistics.stdev(mem) if len(mem) > 1 else 0.0
cpu_mean = statistics.mean(cpu)
cpu_std = statistics.stdev(cpu) if len(cpu) > 1 else 0.0
print(f'$t,{mem_mean:.9f},{mem_std:.9f},{cpu_mean:.9f},{cpu_std:.9f}')
" >> "$OUT"
    rm -f "/tmp/.tarefa04_mem_$t" "/tmp/.tarefa04_cpu_$t"
    echo "Concluído threads=$t"
done

echo "Gerando gráficos..."
python3 plot_results.py
python3 plot_roofline.py

cat "$OUT"
