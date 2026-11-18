# a3 - Synchronization Costs with K-means Locks

This assignment studies mutual exclusion overheads in a shared-memory setting by running K-means under different synchronization strategies.

## Assignment focus
- Compare multiple lock types under the same OpenMP K-means workload.
- Evaluate scaling and lock contention across 1-64 threads.
- Automate benchmarks and generate comparison plots.

The [assignment report](../docs/reports/individual/a3/) details the lock implementations and performance comparisons.

## Contents
- `omp_critical_kmeans.c`, `omp_lock_kmeans.c`, `omp_naive_kmeans.c`: OpenMP variants.
- `locks/`: lock implementations (no-sync, pthread mutex/spin, TAS/TTAS, array, CLH).
- `benchmarks/`: per-lock timing outputs.
- `diagrams/`: plots comparing lock strategies.
- `Makefile`, `run_on_queue.sh`, `make_on_queue.sh`: build and queue scripts.

## Build and validation

The six allocating lock variants use a shared header that checks the requested element count and stops on allocation failure. Their x86-64 object files were rebuilt from the retained lock implementations using GCC 15.2. To rebuild them from the repository root with GCC and POSIX threads:

```bash
make -B -C a3 locks/{array,clh,pthread_mutex,pthread_spin,tas,ttas}_lock.o
```

To link and run the K-means variants, supply the matching [course I/O, driver and utility files](../EXTERNAL_INPUTS.md#local-setup-and-builds) and build with OpenMP:

```bash
make -C a3 all
OMP_NUM_THREADS=4 ./a3/kmeans_omp_pthread_mutex_lock -c 2 -s 0.001 -n 2 -l 3
```

Bounded checks exercised mutual exclusion with one and four workers and verified K-means assignments and centroids on a small synthetic dataset. The no-synchronization control was checked with one worker. The archived benchmark timings describe the original coursework builds.

See the [repository guide](../README.md) for toolchain separation, PBS assumptions, attribution, and measurement limits.
