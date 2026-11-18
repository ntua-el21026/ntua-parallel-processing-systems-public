# a2 - Shared-Memory Parallelism (OpenMP)

This assignment explores parallelization and optimization on shared-memory systems using OpenMP. It includes two subprojects: K-means clustering and Floyd-Warshall all-pairs shortest paths.

## Assignment focus
- K-means: implement naive and reduction-based OpenMP variants and study thread affinity.
- Floyd-Warshall: compare ordinary, recursive and tiled serial traversals with the OpenMP task implementation.
- Collect timing data and compare scalability across thread counts.

The [assignment report](../docs/reports/individual/a2/) summarizes the OpenMP design choices, affinity policies, and performance results.

## Contents
- `kmeans/`: OpenMP K-means implementations, build scripts, and benchmarks.
- [Floyd–Warshall sources](FW/Makefile), queue scripts, and archived benchmarks.
- The [course references](../EXTERNAL_INPUTS.md#course-references) identify the external assignment PDFs.

See the [repository guide](../README.md) for toolchain separation, PBS assumptions, attribution, and measurement limits.

## Local builds

The OpenMP K-means kernels are retained. Linking their executable targets requires the [local course input, driver and utility files](../EXTERNAL_INPUTS.md#local-setup-and-builds) in their documented paths.

## Floyd–Warshall reference programs

Build the four targets with `make -C a2/FW` in a working copy. GCC with
OpenMP is required for the existing parallel target. The three independent
serial references solve the same shortest-path problem with different
memory traversals.

| Target | Arguments | Method |
| --- | --- | --- |
| `fw` | `N` | Completes the distance update one pivot at a time. |
| `fw_sr` | `N B` | Recursively splits each pivot's destination rectangle until its sides fit the leaf size `B`. |
| `fw_tiled` | `N B` | Closes the pivot tile, updates its row and column, then visits the remaining tiles. The last tile can be partial. |
| `fw_sr_p` | `N B` | Runs the retained OpenMP task version. Use powers of two with `B` dividing `N`. |

The serial references reserve `INT_MAX` for unreachable pairs and use
widened arithmetic for the relaxation. Exact finite distances must fit
the integer representation. Negative edges are supported for graphs
without negative cycles. The benchmark executables use the existing
generated input and print elapsed-time records. Archived benchmark
measurements predate the independent serial references.

From the repository root, run
`python3 -B tests/test_independent_algorithms.py -v` to build the targets
in temporary storage and compare the serial references against repeated
edge relaxation on sparse directed graphs.
