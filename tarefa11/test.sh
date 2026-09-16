#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT

for source in v*.c; do
 "${CC:-gcc}" -std=c11 -O2 -Wall -Wextra -Werror -fopenmp \
   "$source" -lm -o "$build/${source%.c}"
done

python3 - "$build" <<'PY'
import os
from pathlib import Path
import subprocess
import sys

build = Path(sys.argv[1])

def run(binary, mode, threads, output):
    result = subprocess.run(
        [str(binary), *([] if mode is None else [str(mode)]), str(output)],
        env={**os.environ, 'OMP_NUM_THREADS': str(threads), 'OMP_DYNAMIC': 'FALSE'},
        check=True,
        stdout=subprocess.PIPE,
        text=True,
    )
    seconds = float(result.stdout.split("tempo=")[1].split()[0])
    assert 0 < seconds < float("inf"), result.stdout
    return output.read_bytes()

seq = build / 'v0_seq'
expected = {}
for mode in (0, 1, 2):
    expected[mode] = run(seq, mode, 1, build / f'expected-{mode}.txt')

assert run(seq, 0, 4, build / 'seq-four.txt') == expected[0]
result = subprocess.run([str(seq), '0'], capture_output=True, text=True, check=True,
                        env={**os.environ, 'OMP_NUM_THREADS': '4'})
assert 'threads=1 ' in result.stdout
assert subprocess.run([str(seq)], capture_output=True, check=True).returncode == 0

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
    for threads in (1, 2, 4):
        output = build / f'{binary.name}-{threads}.txt'
        assert run(binary, None, threads, output) == expected[0]
        checks += 1
    result = subprocess.run([str(binary)], capture_output=True, text=True, check=True,
                            env={**os.environ, 'OMP_NUM_THREADS': '2'})
    assert 'threads=2 ' in result.stdout and 'modo=' not in result.stdout

for binary in [seq, *parallel_binaries]:
    invalid = ([''], ['abc'], ['-1'], ['3'], ['01'], ['+1'], [' 1'], ['1x'],
               ['999999999999999999999'], ['0', '/nonexistent/field'], ['0', 'a', 'b'])
    if binary != seq:
        invalid = ([''], ['/nonexistent/field'], ['0', str(build / 'unexpected.txt')])
    for args in invalid:
        result = subprocess.run(
            [str(binary), *args],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            env={**os.environ, 'OMP_NUM_THREADS': '2'},
        )
        assert result.returncode != 0, (binary, args)

print(f'OK: {checks} comparações gaussianas 512x512; três modos na v0; interfaces e erros de saída.')
PY
