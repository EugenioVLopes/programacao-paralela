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
command -v pascalanalyzer
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
hostname > "$out/host.txt"
lscpu > "$out/cpu.txt"
if command -v numactl >/dev/null 2>&1; then
    numactl --hardware > "$out/numa.txt"
fi
scontrol show job "$SLURM_JOB_ID" > "$out/job.txt"
pascalanalyzer -h > "$out/help.txt"
PASCAL=1 bash "$repo_root/tarefa12/build.sh" "$out/bin" > "$out/build.txt" 2>&1
sha256sum "$repo_root/tarefa12/v4_numa.c" "$repo_root/tarefa12/build.sh" \
    "$repo_root/tarefa12/pascal.slurm" "$task_dir/afinidade.slurm" > "$out/sha256.txt"
base="$out/bin/v4_numa"
cp -p "$base" "$out/bin/v4_numa_false"
cp -p "$base" "$out/bin/v4_numa_close_cores"
cp -p "$base" "$out/bin/v4_numa_close_threads"
cp -p "$base" "$out/bin/v4_numa_spread_threads"
run() {
    local cfg="$1"; shift
    local env_bind="$1"; shift
    local places="$1"; shift
    local bin="$out/bin/v4_numa_${cfg}"
    export OMP_PROC_BIND="$env_bind"
    if [ "$places" = "-" ]; then
        unset OMP_PLACES
    else
        export OMP_PLACES="$places"
    fi
    export OMP_DISPLAY_ENV=VERBOSE
    export cfg_env="$out/env_${cfg}.txt"
    export cfg_cpuset="$out/cpuset_${cfg}.txt"
    export cfg_bin="$bin"
    export cfg_json="$out/data/v4_numa_${cfg}.json"
    srun --ntasks=1 --cpus-per-task="$SLURM_CPUS_PER_TASK" --cpu-bind=verbose,cores \
        bash -c '
            env | grep "^OMP_" > "$cfg_env"
            { taskset -pc "$$" 2>&1 || true; grep -E "Cpus_allowed|Mems_allowed" /proc/self/status 2>/dev/null || true; } > "$cfg_cpuset"
            exec pascalanalyzer "$cfg_bin" -t man -r 1 -c 1,2,4,8,16,32,64 \
                -i 262144,524288,1048576,2097152,4194304,8388608,16777216 \
                -o "$cfg_json"
        '
}
run false false -
run close_cores close cores
run close_threads close threads
run spread_threads spread threads
