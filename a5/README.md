# a5 - GPU K-means with CUDA

This assignment moves K-means clustering to NVIDIA GPUs using CUDA. The goal is to compare increasingly optimized GPU kernels and quantify the impact of memory access patterns and host-device transfers.

## Assignment focus
- Naive GPU offload (membership on GPU, centroid update on CPU).
- Data layout optimization (transpose/column-major) for memory coalescing.
- Shared-memory centroids to reduce global memory traffic.
- Full GPU version with atomics for centroid updates.

The [assignment report](../docs/reports/individual/a5/) walks through each kernel variant and the performance analysis.

## Contents
- `cuda_kmeans_naive.cu`, `cuda_kmeans_transpose.cu`, `cuda_kmeans_shared.cu`, `cuda_kmeans_all_gpu.cu`: kernel variants.
- `main_gpu.cu`, `main_sec.c`, `seq_kmeans.c`: drivers and reference code.
- `Makefile`, `make_on_queue.sh`, `run_on_queue.sh`: build and queue scripts.
- `validation_results.txt`, `validate_on_queue.sh`: correctness checks.
- `diagrams/`, `Execution_logs/`: plots and run logs.

## Build status

The serial target uses G++. The independent
[host generator](Helper_functions/file_io.c) returns contiguous row-major
`double` coordinates in `[0, 1)` through the existing `dataset_generation`
interface. It uses a fixed local seed for repeatable CPU and GPU checks.
The generated points and any new timings have a separate workload
identity from the archived measurements.

Supply the authorized [local helper set](../EXTERNAL_INPUTS.md#local-setup-and-builds)
before building. The serial target needs the external utility source.
The Makefile creates `ObjectDir`. In a disposable working copy, ensure
`Execution_logs/` is writable.

With the exact helper sources and declarations supplied, `make all NVCC=/path/to/nvcc` builds the four implemented CUDA variants. Review the historical `sm_60` flags against the local toolkit and GPU.
`cuda_kmeans_all_gpu_delta_reduction.cu` has unfinished thread-index/reduction
scaffolding and is excluded from `all`. `make experimental` is explicitly opt-in
for investigation, without an algorithmic correctness claim.

The bounded CPU and build-graph regressions under `tests/test_a5_build.py`
use the documented companion sources. Run
`python3 -B tests/test_independent_algorithms.py -v` from the repository root
to check deterministic values, the G++ interface and invalid dimensions
without those inputs. Compare numerical results against the serial
reference before measuring a CUDA variant.

See the [repository guide](../README.md) for toolchain separation, PBS assumptions, attribution, and measurement limits.
