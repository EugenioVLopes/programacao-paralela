#!/bin/bash
#SBATCH --job-name=tarefa15
#SBATCH --output=runs/%x-%j.out
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --partition=intel-128

set -euo pipefail

echo "Job ID: $SLURM_JOB_ID"
echo "Processos: $SLURM_NTASKS"
echo "Nos: $SLURM_JOB_NUM_NODES"
echo "Lista de nos:"
scontrol show hostnames "$SLURM_JOB_NODELIST"

binary="tarefa15-${SLURM_JOB_ID}.bin"
trap 'rm -f "$binary"' EXIT

mpicc -std=c11 -O3 -Wall -Wextra -o "$binary" tarefa15.c

for run in $(seq 1 10); do
    echo "Execucao $run"
    mpirun -np "$SLURM_NTASKS" "./$binary"
done
