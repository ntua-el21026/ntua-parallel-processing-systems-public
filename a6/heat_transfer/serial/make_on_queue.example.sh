#!/bin/bash
#PBS -N heat_serial_build
#PBS -l walltime=00:05:00

set -euo pipefail

work_dir=${HEAT_SERIAL_DIR:-${PBS_O_WORKDIR:-}}
if [[ -z $work_dir ]]; then
    work_dir=$(dirname -- "${BASH_SOURCE[0]}")
fi
cd -- "$work_dir"

if [[ ! -f Makefile || ! -f utils.c ]]; then
    printf 'Expected the serial heat-transfer source directory: %s\n' "$PWD" >&2
    exit 2
fi

exec make all
