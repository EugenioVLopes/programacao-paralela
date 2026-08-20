#!/bin/bash
gcc tarefa01.c -o tarefa01 -lm

ITERACOES=(1 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000 10000000000 100000000000)
JSON="["

for i in "${!ITERACOES[@]}"; do
    N=${ITERACOES[$i]}

    START=$(date +%s%N)
    OUTPUT=$(./tarefa01 "$N")
    END=$(date +%s%N)

    PI=$(echo "$OUTPUT" | grep "Pi:" | awk '{print $2}')
    ERRO=$(echo "$OUTPUT" | grep "Erro:" | awk '{print $2}')
    RAW_TEMPO=$(echo "scale=6; ($END - $START) / 1000000000" | bc)
    TEMPO=$(printf "%f" "$RAW_TEMPO")

    [ $i -gt 0 ] && JSON+=","
    JSON+="{\"iteracoes\":$N,\"pi\":$PI,\"erro\":\"$ERRO\",\"tempo\":$TEMPO}"
done

JSON+="]"
echo "$JSON" | python3 -m json.tool > results.json
cat results.json
