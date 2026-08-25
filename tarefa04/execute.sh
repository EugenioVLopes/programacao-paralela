#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-50000000}"
OUT="results.csv"
MAX_THREADS=$(nproc)
REPETICOES="${REPETICOES:-3}"

printf 'threads,memory_bound,cpu_bound\n' > "$OUT"

gcc -std=c11 -Wall -Wextra -fopenmp tarefa04.c -o tarefa04 -lm

echo "Iniciando experimento com N=$N e max_threads=$MAX_THREADS"

mediana() {
    sort -n | awk '{v[NR]=$1} END {if (NR % 2) print v[(NR+1)/2]; else print (v[NR/2]+v[NR/2+1])/2}'
}

for ((t=1; t<=MAX_THREADS; t++)); do
    export OMP_NUM_THREADS=$t
    export OMP_DYNAMIC=false
    memoria_tempos=()
    cpu_tempos=()
    for ((r=1; r<=REPETICOES; r++)); do
        valores=$(./tarefa04 "$N" | sed -E 's/.*memory_bound=([^ ]+) cpu_bound=([^ ]+).*/\1,\2/')
        memoria_tempos+=("${valores%,*}")
        cpu_tempos+=("${valores#*,}")
    done
    memoria=$(printf '%s\n' "${memoria_tempos[@]}" | mediana)
    cpu=$(printf '%s\n' "${cpu_tempos[@]}" | mediana)
    printf '%d,%s,%s\n' "$t" "$memoria" "$cpu" >> "$OUT"
    echo "Concluído threads=$t ($REPETICOES repetições; mediana registrada)"
done

rm -f tarefa04

echo "Gerando gráficos..."
python3 plot_results.py

cat "$OUT"
