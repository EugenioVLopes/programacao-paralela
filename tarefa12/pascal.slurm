#!/usr/bin/env bash
#SBATCH --job-name=t12-pascal
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=amd-512
#SBATCH --cpus-per-task=64
#SBATCH --hint=nomultithread
#SBATCH --exclusive
#SBATCH --mem=8G
#SBATCH --time=02:00:00
#SBATCH --output=pascal-%j.out
set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?Submeta com sbatch, dentro de tarefa12}"
# PASCAL_ENV deve apontar para env.sh da instalacao no NPAD.
# A distribuição acessa LD_LIBRARY_PATH sob nounset; inicialize-a antes do source.
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}"
set +u
pascal_root="$(cd "$(dirname "${PASCAL_ENV:?Defina PASCAL_ENV com o caminho absoluto de env.sh}")" && pwd)"
pushd "$pascal_root" >/dev/null
source "$pascal_root/env.sh"
popd >/dev/null
set -euo pipefail
export OMP_DYNAMIC=FALSE OMP_PLACES=cores
export OMP_PROC_BIND=${OMP_PROC_BIND:-spread}
export OMP_WAIT_POLICY=${OMP_WAIT_POLICY:-PASSIVE}
unset GOMP_CPU_AFFINITY KMP_AFFINITY
if (( SLURM_CPUS_PER_TASK < 64 )); then
    echo 'Este desenho requer 64 CPUs alocadas: p=1,2,4,8,16,32,64' >&2
    exit 1
fi
out="runs/pascal-$SLURM_JOB_ID"
mkdir -p runs
mkdir "$out" # Recusa reutilizar o mesmo diretorio.
mkdir "$out/data"
PASCAL=1 bash build.sh "$out/bin"
# WORKLOAD_INPUT recebe alvo de celulas, nao o lado N.
# 500 passos e modo gaussiano sao os defaults do executavel.
for version in v1_barriers v2_private v3_simd v4_numa; do
    srun --ntasks=1 --cpus-per-task="$SLURM_CPUS_PER_TASK" --cpu-bind=cores \
        pascalanalyzer "./$out/bin/$version" -t man -r "${REPS:-5}" \
        -c 1,2,4,8,16,32,64 -i 262144,524288,1048576,2097152,4194304,8388608,16777216 \
        -o "$out/data/$version.json"
done
