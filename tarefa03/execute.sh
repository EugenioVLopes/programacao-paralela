#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
N="${1:-100000000}"
OUT="results.csv"
printf 'otimizacao,inicializacao,dependente,multiplos\n' > "$OUT"

for O in O0 O2 O3; do
    gcc -std=c11 -Wall -Wextra -O"${O#O}" tarefa03.c -o "tarefa03_${O}"
    linha=$(./"tarefa03_${O}" "$N")
    valores=$(printf '%s\n' "$linha" | sed -E 's/.*inicializacao=([^ ]+) dependente=([^ ]+) multiplos=([^ ]+).*/\1,\2,\3/')
    printf '%s,%s\n' "$O" "$valores" >> "$OUT"
    rm -f "tarefa03_${O}"
done

cat "$OUT"
