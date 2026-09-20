#!/bin/bash
#SBATCH --job-name=tarefa15
#SBATCH --output=tarefa15-%j.out
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --partition=intel-512

set -e
mpicc -std=c11 -O3 -Wall -Wextra -o tarefa15 tarefa15.c
for run in 1 2 3 4 5; do
    echo "Execucao $run"
    mpirun -np "$SLURM_NTASKS" ./tarefa15
done
