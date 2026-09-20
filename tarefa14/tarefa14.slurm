#!/bin/bash
#SBATCH --job-name=PINGPONG
#SBATCH --output=tarefa14-%j.out
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=1
#SBATCH --time=01:00:00
#SBATCH --partition=intel-512

set -e

mpicc -std=c11 -O3 -Wall -Wextra -o tarefa14 tarefa14.c
mpirun -np 2 ./tarefa14
