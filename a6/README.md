# a6 - Distributed Memory (MPI)

This assignment targets distributed-memory parallelism using MPI. It includes two workloads: K-means clustering and 2D heat transfer.

## Assignment focus
- MPI K-means: distribute points, compute local contributions, and combine centroids with collective reductions.
- 2D heat transfer: grid decomposition with halo exchanges and convergence checks (Jacobi, Gauss–Seidel and Red–Black variants, with additional source assembly needed).
- Measure total time vs computation time and study speedup across process counts.

The [assignment report](../docs/reports/individual/a6/) summarizes the MPI design, timing methodology, and scalability results.

## Contents
- `kmeans/`: MPI K-means implementation, benchmarks, and run scripts.
- `heat_transfer/`: MPI heat transfer kernels and benchmarks (Jacobi, Gauss-Seidel SOR, Red-Black SOR).
- `diagrams/`: plotting scripts and generated figures.
- The [course references](../EXTERNAL_INPUTS.md#course-references) identify the external assignment PDFs.

See the [repository guide](../README.md) for toolchain separation, PBS assumptions, attribution, and measurement limits.

## Build and scheduler notes

After using the [source assembly instructions](../course-source-reconstruction/README.md) and restoring the [matching external inputs](../EXTERNAL_INPUTS.md#local-setup-and-builds), use `make -C a6/kmeans` for `kmeans_mpi` and `make -C a6/heat_transfer/mpi` for `jacobi_mpi`, `gauss_mpi`, and `redblack_mpi`. The latter Makefile's target names are `jacobi`, `gauss`, and `redblack`; executable names include the `_mpi` suffix. `make -C a6/heat_transfer/serial` requires the separate course serial implementations. The independent [serial utilities](heat_transfer/serial/utils.c)
and [MPI utilities](heat_transfer/mpi/utils.c) share the same interface.
Each allocates contiguous storage with a constant row stride for MPI
vector datatypes. Initialization sets a cold interior and a fixed unit
temperature at the logical grid boundary. The convergence check compares
the absolute change against the tolerance over inclusive bounds; MPI
combines local decisions through the existing solver reduction. The
unit-temperature fixture defines new runs. The archived timings and
reported midpoint values describe the historical coursework runs. The
exact historical boundary profile remains unverified.

## Serial heat-transfer jobs

Use the [build job](heat_transfer/serial/make_on_queue.example.sh) and [run job](heat_transfer/serial/run_on_queue.example.sh) with Bash and GNU Make. Both select the working directory from `HEAT_SERIAL_DIR` or `PBS_O_WORKDIR`, in that order. With neither variable set they use the directory containing the script. Restore the three authorized serial solver sources identified in the [input register](../EXTERNAL_INPUTS.md#local-setup-and-builds) before building. The run job expects the binaries `jacobi`, `seidelsor`, and `redblacksor`.

For a local run, use a disposable working copy. From its project root:

```bash
bash a6/heat_transfer/serial/make_on_queue.example.sh
HEAT_GRID_SIZE=16 bash a6/heat_transfer/serial/run_on_queue.example.sh
```

The default is a square grid of size 256. Set `HEAT_GRID_SIZE` to an integer from 3 to 999. The upper bound fits the original solvers' result filename buffers. With the default Makefile settings, the programs write grid results in the selected directory. Reruns with the same dimensions can overwrite those files.

For PBS, submit the build job from the serial source directory. After it completes successfully, submit the run job from the same directory. Request queue, resource and walltime values appropriate to the installation and grid.

```bash
qsub make_on_queue.example.sh
# After the build job completes successfully:
qsub -v HEAT_GRID_SIZE=256 run_on_queue.example.sh
```

From the repository root, run
`python3 -B tests/test_independent_algorithms.py -v` for utility tests
covering storage layout, padding, initialization, convergence and
numeric output in both source directories.

The two original templates, [make_on_queue.sh](heat_transfer/serial/make_on_queue.sh) and [run_on_queue.sh](heat_transfer/serial/run_on_queue.sh), preserve the historical `cd <FIX PATH>` placeholders. Other recorded PBS scripts use the original course modules and host assumptions.
