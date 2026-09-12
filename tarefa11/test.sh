#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT

for source in v*.c; do
 flags=()
 [[ $source == v0* ]] || flags=(-fopenmp)
 "${CC:-gcc}" -std=c11 -O2 -Wall -Wextra -Werror "${flags[@]}" \
   "$source" -lm -o "$build/${source%.c}"
done

python3 - "$build" <<'PY'
import os
from pathlib import Path
import subprocess
import sys

build = Path(sys.argv[1])

def run(binary, mode, threads, output):
    subprocess.run(
        [str(binary), str(mode), str(output)],
        env={**os.environ, 'OMP_NUM_THREADS': str(threads), 'OMP_DYNAMIC': 'FALSE'},
        check=True,
        stdout=subprocess.DEVNULL,
    )
    return output.read_bytes()

seq = build / 'v0_seq'
expected = {}
for mode in (0, 1, 2):
    expected[mode] = run(seq, mode, 1, build / f'expected-{mode}.txt')

values_zero = [float(x) for x in expected[1].split()]
values_one = [float(x) for x in expected[2].split()]
values_gaussian = [float(x) for x in expected[0].split()]
assert len(values_gaussian) == 512 * 512
assert all(x == 0.0 for x in values_zero)
assert all(x == 1.0 for x in values_one)
assert min(values_gaussian) >= 0.0
assert max(values_gaussian) < 0.1

checks = 0
parallel_binaries = [
    build / 'v1_static',
    build / 'v2_static_collapse',
    build / 'v3_dynamic',
    build / 'v4_dynamic_collapse',
    build / 'v5_guided_collapse',
    build / 'v6_guided',
]
for binary in parallel_binaries:
    for mode in (0, 1, 2):
        thread_counts = (1, 2, 4) if mode == 0 else (4,)
        for threads in thread_counts:
            output = build / f'{binary.name}-{mode}-{threads}.txt'
            assert run(binary, mode, threads, output) == expected[mode]
            checks += 1

for binary in [seq, *parallel_binaries]:
    for args in (['abc'], ['-1'], ['3'], ['0', '/nonexistent/field'], ['0', 'a', 'b']):
        result = subprocess.run(
            [str(binary), *args],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            env={**os.environ, 'OMP_NUM_THREADS': '2'},
        )
        assert result.returncode != 0, (binary, args)

print(f'OK: {checks} comparações de campos 512x512; campos parado, constante e perturbado.')
PY
