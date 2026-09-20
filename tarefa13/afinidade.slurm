#!/usr/bin/env bash
#SBATCH --job-name=t13-afinidade
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=amd-512
#SBATCH --cpus-per-task=64
#SBATCH --hint=nomultithread
#SBATCH --exclusive
#SBATCH --mem=8G
#SBATCH --time=04:00:00
#SBATCH --output=afinidade-%j.out
set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?Submeta com sbatch, dentro de tarefa13}"
task_dir="$(pwd -P)"
repo_root="$(cd .. && pwd -P)"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}"
set +u
pascal_root="$(cd "$(dirname "${PASCAL_ENV:?Defina PASCAL_ENV com o caminho absoluto de env.sh}")" && pwd)"
pushd "$pascal_root" >/dev/null
source "$pascal_root/env.sh"
popd >/dev/null
set -euo pipefail
export OMP_DYNAMIC=FALSE OMP_WAIT_POLICY=PASSIVE
unset GOMP_CPU_AFFINITY KMP_AFFINITY
if (( SLURM_CPUS_PER_TASK < 64 )); then
    echo 'Este desenho requer 64 CPUs alocadas: p=1,2,4,8,16,32,64' >&2
    exit 1
fi
out="$task_dir/runs/pascal-$SLURM_JOB_ID"
mkdir -p "$task_dir/runs"
mkdir "$out"
mkdir "$out/data"
PASCAL=1 bash "$repo_root/tarefa12/build.sh" "$out/bin"
for cfg in false close_cores close_threads spread_threads; do
  cp -p "$out/bin/v4_numa" "$out/bin/v4_numa_${cfg}"
done
run() {
  local cfg="$1" bind="$2" places="$3"
  export OMP_PROC_BIND="$bind"
  if [ "$places" = "-" ]; then
    unset OMP_PLACES
  else
    export OMP_PLACES="$places"
  fi
  srun --ntasks=1 --cpus-per-task="$SLURM_CPUS_PER_TASK" --cpu-bind=cores \
    pascalanalyzer "$out/bin/v4_numa_${cfg}" -t man -r 1 -c 1,2,4,8,16,32,64 \
      -i 262144,524288,1048576,2097152,4194304,8388608,16777216 \
      -o "$out/data/v4_numa_${cfg}.json"
}
run false false -
run close_cores close cores
run close_threads close threads
run spread_threads spread threads
