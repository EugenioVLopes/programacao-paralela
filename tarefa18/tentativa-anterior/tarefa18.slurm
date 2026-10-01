#!/bin/bash
#SBATCH --job-name=tarefa18
#SBATCH --output=tarefa18-%j.out
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu:1
#SBATCH --time=00:15:00
#SBATCH --partition=gpu-4-a100

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?Submeta o job a partir do diretório tarefa18}"

ulimit -s unlimited # vetores na pilha: sem isto, segfault
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_TARGET_OFFLOAD=MANDATORY # sem offload o job falha em vez de medir a CPU 2x

results="medicoes-${SLURM_JOB_ID}.csv"
printf 'N,dispositivos,na_gpu,err_cpu,err_gpu,repeticao,cpu_s,gpu_s\n' > "$results"
for tamanho in 1000000 10000000 50000000; do
    binary="tarefa18-${tamanho}-${SLURM_JOB_ID}.bin"
    gcc -std=c11 -O3 -Wall -Wextra -Wpedantic -fopenmp \
        -DN="$tamanho" -o "$binary" tarefa18.c
    for repeticao in 1 2 3; do
        echo "N=${tamanho} repeticao=${repeticao}"
        campos=$("./$binary" | awk -F, -v r="$repeticao" \
            'NF == 7 { print $1","$2","$3","$4","$5","r","$6","$7; exit }')
        [[ -n "$campos" ]] || { echo "CSV invalido (N=${tamanho})." >&2; exit 1; }
        printf '%s\n' "$campos" >> "$results"
    done
    rm -f "$binary"
done
