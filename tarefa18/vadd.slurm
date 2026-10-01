#!/bin/bash
#SBATCH --partition=gpu-8-v100
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu:1
#SBATCH --time=00:05:00
#SBATCH --job-name=tarefa18
#SBATCH --output=vadd-%j.out

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
ulimit -s unlimited
export OMP_STACKSIZE=256M
export OMP_NUM_THREADS="$SLURM_CPUS_PER_TASK"
export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_TARGET_OFFLOAD=MANDATORY

module load compilers/nvidia/nvhpc/24.11
command -v nvc
hostname
lscpu
nvidia-smi || echo "Aviso: nvidia-smi retornou erro; a execucao GPU sera verificada pelo programa."
nvc --version
sha256sum vadd_cpu.c vadd_gpu.c
nvc -O3 -mp=multicore vadd_cpu.c -o vadd_cpu
nvc -O3 -mp=gpu -Minfo=mp vadd_gpu.c -o vadd_gpu

for repeticao in 1 2 3 4 5; do
    echo "Repeticao $repeticao CPU; threads=$OMP_NUM_THREADS"
    ./vadd_cpu
    echo "Repeticao $repeticao GPU"
    ./vadd_gpu
done
