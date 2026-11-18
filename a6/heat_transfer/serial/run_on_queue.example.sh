#!/bin/bash
#PBS -N heat_serial_run
#PBS -l walltime=00:05:00

set -euo pipefail

work_dir=${HEAT_SERIAL_DIR:-${PBS_O_WORKDIR:-}}
if [[ -z $work_dir ]]; then
    work_dir=$(dirname -- "${BASH_SOURCE[0]}")
fi
cd -- "$work_dir"

grid_size=${HEAT_GRID_SIZE:-256}
# The historical solvers use fixed-size buffers for their grid output filenames.
if [[ ! $grid_size =~ ^[1-9][0-9]{0,2}$ || $grid_size -lt 3 ]]; then
    printf 'HEAT_GRID_SIZE must be an integer from 3 to 999.\n' >&2
    exit 2
fi

programs=(jacobi seidelsor redblacksor)
for program in "${programs[@]}"; do
    if [[ ! -x ./$program ]]; then
        printf 'Expected executable %s in %s. Build it first.\n' "$program" "$PWD" >&2
        exit 2
    fi
done
for program in "${programs[@]}"; do
    "./$program" "$grid_size"
done
