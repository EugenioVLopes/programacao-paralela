#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

N="${1:-5000000}"
NUM_RUNS="${2:-10}"
RAW_OUT="raw_results.csv"
OUT="results.csv"
ENV_OUT="environment.txt"
MAX_THREADS=$(nproc)
PHYSICAL_CORES=$(lscpu -p=CORE,SOCKET 2>/dev/null \
    | awk -F, '!/^#/ { print $1 "," $2 }' \
    | sort -u \
    | wc -l \
    | tr -d ' ')

if ! [[ "$PHYSICAL_CORES" =~ ^[1-9][0-9]*$ ]]; then
    PHYSICAL_CORES="$MAX_THREADS"
fi

if ! [[ "$N" =~ ^[1-9][0-9]*$ ]]; then
    echo "Erro: N deve ser um inteiro positivo." >&2
    exit 1
fi

if ! [[ "$NUM_RUNS" =~ ^[1-9][0-9]*$ ]]; then
    echo "Erro: NUM_RUNS deve ser um inteiro positivo." >&2
    exit 1
fi

# Use THREADS para escolher manualmente as configuracoes.
# Exemplo: THREADS="1 2 4 8 16" ./execute.sh 50000000 7
if [[ -n "${THREADS:-}" ]]; then
    THREAD_LIST="$THREADS"
else
    # Mede cada configuracao disponivel, como nas tarefas anteriores.
    # A lista completa permite observar a transicao para threads logicas.
    THREAD_LIST=$(seq 1 "$MAX_THREADS" | paste -sd ' ' -)
fi

for threads in $THREAD_LIST; do
    if ! [[ "$threads" =~ ^[1-9][0-9]*$ ]] || (( threads > MAX_THREADS )); then
        echo "Erro: numero de threads invalido: $threads" >&2
        exit 1
    fi
done

# Desabilita ajuste automatico e fixa as threads em cores para reduzir a
# variacao introduzida pelo runtime e pelo escalonador do sistema.
export OMP_DYNAMIC="${OMP_DYNAMIC:-FALSE}"
export OMP_PROC_BIND="${OMP_PROC_BIND:-close}"
export OMP_PLACES="${OMP_PLACES:-cores}"

BINARIOS=(e0 e1 e2 e3)
TMP_DIR=$(mktemp -d)

cleanup() {
    rm -rf "$TMP_DIR"
    rm -f "${BINARIOS[@]}"
}
trap cleanup EXIT

for programa in "${BINARIOS[@]}"; do
    gcc -std=c11 -O2 -Wall -Wextra -fopenmp "$programa.c" -o "$programa" -lm
done

{
    echo "Data: $(date --iso-8601=seconds)"
    echo "Hostname: $(hostname)"
    echo "N: $N"
    echo "Repeticoes por configuracao: $NUM_RUNS"
    echo "Threads testadas: $THREAD_LIST"
    echo "Nucleos fisicos detectados: $PHYSICAL_CORES"
    echo "Threads logicas detectadas: $MAX_THREADS"
    echo "OMP_DYNAMIC: $OMP_DYNAMIC"
    echo "OMP_PROC_BIND: $OMP_PROC_BIND"
    echo "OMP_PLACES: $OMP_PLACES"
    echo
    echo "GCC:"
    gcc --version | head -n 1
    echo
    echo "CPU:"
    lscpu
} > "$ENV_OUT"

./e0 "$N" > sequential_result.txt
printf 'version,threads,run,pi,error,time_s\n' > "$RAW_OUT"

extrair_pi() {
    awk -F ' = ' '$1 == "PI" { print $2 }'
}

extrair_erro() {
    awk -F ' = ' '$1 == "Erro" { print $2 }'
}

extrair_tempo() {
    awk -F ' = ' '$1 == "Tempo" { split($2, campos, " "); print campos[1] }'
}

for threads in $THREAD_LIST; do
    echo "Threads: $threads"

    # e1 nao tem sincronizacao. Repeticoes revelam a variacao causada pela
    # condicao de corrida, pois cada execucao usa os mesmos pontos amostrados.
    for ((run = 1; run <= NUM_RUNS; run++)); do
        output=$(OMP_NUM_THREADS="$threads" ./e1 "$N")
        pi=$(printf '%s\n' "$output" | extrair_pi)
        erro=$(printf '%s\n' "$output" | extrair_erro)
        printf 'e1,%d,%d,%s,%s,\n' "$threads" "$run" "$pi" "$erro" >> "$RAW_OUT"
    done

    # Uma execucao curta antes da medicao evita que a primeira chamada de e2
    # ou e3 carregue sozinha custos de inicializacao do runtime OpenMP.
    OMP_NUM_THREADS="$threads" ./e3 "$N" >/dev/null

    for programa in e2 e3; do
        for ((run = 1; run <= NUM_RUNS; run++)); do
            output=$(OMP_NUM_THREADS="$threads" ./$programa "$N")
            pi=$(printf '%s\n' "$output" | extrair_pi)
            erro=$(printf '%s\n' "$output" | extrair_erro)
            tempo=$(printf '%s\n' "$output" | extrair_tempo)
            printf '%s,%d,%d,%s,%s,%s\n' \
                "$programa" "$threads" "$run" "$pi" "$erro" "$tempo" >> "$RAW_OUT"
        done
    done
done

python3 - "$RAW_OUT" "$OUT" <<'PY'
import csv
import statistics
import sys
from collections import defaultdict

raw_path, out_path = sys.argv[1:]
values = defaultdict(list)

with open(raw_path, newline="", encoding="utf-8") as raw_file:
    for row in csv.DictReader(raw_file):
        key = (int(row["threads"]), row["version"])
        values[key].append({
            "pi": float(row["pi"]),
            "error": float(row["error"]),
            "time": float(row["time_s"]) if row["time_s"] else None,
        })

def mean(items):
    return statistics.fmean(items)

def stdev(items):
    return statistics.stdev(items) if len(items) > 1 else 0.0

thread_counts = sorted({threads for threads, _ in values})
header = [
    "threads",
    "e1_pi_mean", "e1_pi_std", "e1_pi_min", "e1_pi_max",
    "e2_pi", "e2_error", "e2_time_median_s", "e2_time_mean_s", "e2_time_std_s",
    "e3_pi", "e3_error", "e3_time_median_s", "e3_time_mean_s", "e3_time_std_s",
    "e2_e3_time_ratio",
]

with open(out_path, "w", newline="", encoding="utf-8") as out_file:
    writer = csv.writer(out_file)
    writer.writerow(header)

    for threads in thread_counts:
        e1 = values[(threads, "e1")]
        e2 = values[(threads, "e2")]
        e3 = values[(threads, "e3")]

        e1_pi = [result["pi"] for result in e1]
        e2_time = [result["time"] for result in e2]
        e3_time = [result["time"] for result in e3]
        e2_median = statistics.median(e2_time)
        e3_median = statistics.median(e3_time)

        writer.writerow([
            threads,
            f"{mean(e1_pi):.10f}", f"{stdev(e1_pi):.10f}",
            f"{min(e1_pi):.10f}", f"{max(e1_pi):.10f}",
            f"{e2[0]['pi']:.10f}", f"{e2[0]['error']:.10e}",
            f"{e2_median:.6f}", f"{mean(e2_time):.6f}", f"{stdev(e2_time):.6f}",
            f"{e3[0]['pi']:.10f}", f"{e3[0]['error']:.10e}",
            f"{e3_median:.6f}", f"{mean(e3_time):.6f}", f"{stdev(e3_time):.6f}",
            f"{e2_median / e3_median:.2f}",
        ])
PY

echo "Gerando graficos..."
python3 plot_results.py "$OUT" "$ENV_OUT"

cat "$OUT"
echo
echo "Arquivos gerados: $OUT, $RAW_OUT, sequential_result.txt, $ENV_OUT e os graficos PNG/PDF."
