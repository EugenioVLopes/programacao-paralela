#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
dest=${1:-build}
mkdir -p "$dest"
flags=(-std=c11 -fopenmp -O2 -Wall -Wextra -Werror -ffp-contract=off)
[[ ${NATIVE:-0} == 0 ]] || flags+=(-march=native)
libs=(-lm)
if [[ ${PASCAL:-0} == 1 ]]; then
    flags+=(-DUSE_PASCAL -DWORKLOAD_INPUT)
    libs+=(-lmpascalops)
fi
for src in v*.c; do
    cmd=("${CC:-gcc}" "${flags[@]}" "$src" "${libs[@]}" -o "$dest/${src%.c}")
    printf '%q ' "${cmd[@]}"
    printf '\n'
    "${cmd[@]}"
done
