#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

gcc -std=c11 -Wall -Wextra -O0 row_major.c -o row_major
gcc -std=c11 -Wall -Wextra -O0 column_major.c -o column_major

NS=(10 20 30 40 50 100 200 300 400 500 600 700 800 900 1000 1200 1400 1600 1800 2000 2500 3000 3500 4000 4500 5000)
JSON="["

for i in "${!NS[@]}"; do
    N=${NS[$i]}
    
    row_out=$(./row_major "$N")
    row_time=$(echo "$row_out" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')
    
    col_out=$(./column_major "$N")
    col_time=$(echo "$col_out" | sed -E 's/.*tempo=([0-9.]+) s.*/\1/')
    
    [ "$i" -gt 0 ] && JSON+=","
    JSON+="{\"N\": $N, \"row_time\": $row_time, \"col_time\": $col_time}"
done

JSON+="]"
echo "$JSON" | python3 -m json.tool > results.json
cat results.json

if command -v python3 &>/dev/null; then
    python3 plot_results.py || true
fi

rm -f row_major column_major
