# Parallel Processing Systems

**Michail Athanasios Peppas** and **Andreas Saounatsos** investigated shared-memory, GPU, and distributed-memory parallelism across six NTUA ECE assignments. The work includes OpenMP grid and clustering kernels, CUDA memory-layout and reduction strategies, MPI collective and halo-exchange workflows, and workload-specific timing analysis. The [combined report](docs/reports/final/parlab05_report_a1-a6_final.pdf) records methods and results across the six assignments.

| Assignment | Technical work | Evidence |
| --- | --- | --- |
| [Game of Life](a1/README.md) | Parallelize grid updates with OpenMP and compare problem sizes and thread counts. | [Assignment report](docs/reports/individual/a1/parlab05_report_a1_final.pdf) |
| [OpenMP K-means and Floyd–Warshall](a2/README.md) | Compare naive and reduction-based clustering, affinity settings, and the parallel shortest-path variant. | [Assignment report](docs/reports/individual/a2/parlab05_report_a2_final.pdf) |
| [Synchronization costs](a3/README.md) | Compare critical sections and multiple lock implementations in the same clustering workload. | [Assignment report](docs/reports/individual/a3/parlab05_report_a3_final.pdf) |
| [Concurrent sorted list](a4/README.md) | Evaluate contention and throughput across a serial baseline, two locking granularities, and optimistic, lazy, and nonblocking variants. | [Assignment report](docs/reports/individual/a4/parlab05_report_a4_final.pdf) |
| [CUDA K-means](a5/README.md) | Compare offload, transposed layout, shared-memory centroids, and device-side atomic accumulation. | [Assignment report](docs/reports/individual/a5/parlab05_report_a5_final.pdf), [validation record](a5/validation_results.txt) |
| [MPI K-means and heat transfer](a6/README.md) | Study distributed clustering, halo exchange, convergence, and scalability. | [Assignment report](docs/reports/individual/a6/parlab05_report_a6_final.pdf) |

## Source scope

The serial Floyd–Warshall references, six sorted-list variants, their runtime support, and selected CUDA and heat-transfer helpers have independent implementations at the paths used by the assignment builds. The [source register](EXTERNAL_INPUTS.md#retained-source-provenance) explains their purpose and the inputs that still come from the course. The archived reports and benchmark results describe the original submissions and course components.

The available project work includes the Floyd–Warshall OpenMP implementation, OpenMP clustering kernels and lock work, four CUDA variants, the MPI K-means kernel, and the student changes preserved as source-assembly records. Obtain the matching course sources separately when reproducing an exercise. The evidence collection preserves the original reports, timing logs, and plots. [Course references and reports](docs/README.md)

## Local verification and reproduction

The project uses per-assignment builds. Use the project root for the commands below. The GitHub clone command uses the following repository URL.

```bash
git clone https://github.com/ntua-el21026/ntua-parallel-processing-systems-public.git
cd ntua-parallel-processing-systems-public
```

The [Floyd–Warshall build instructions](a2/README.md#local-builds) cover the serial, blocked and OpenMP targets.

Full K-means builds require the external I/O, driver, or utility sources identified for each exercise. For Game of Life and the MPI variants, apply the [hash-verified source assembly recipes](course-source-reconstruction/README.md). The CUDA builds require a compatible NVIDIA toolkit and course helper sources attributed to NTUA CSLab and Vasileios Karakasis. The versioned course examples locate these files under `a5/Helper_functions/`. The experimental CUDA delta-reduction target remains separate from the four implemented variants.

The [concurrent-list guide](a4/README.md) describes the six synchronization strategies, GNU C11 and pthread prerequisites, and regression checks. The benchmark uses a C11 atomic stop flag. Published throughput tables were collected with the historical implementations.

With the exact external helpers, the two Assignment 5 regressions cover the serial CPU run and CUDA build graph. The [serial heat-transfer examples](a6/README.md#serial-heat-transfer-jobs) document local runs and PBS submission. GPU, MPI and scheduler executions require their respective toolchains and environments. The [archived access instructions](shell_scripts/README.md) document the site-specific account and filesystem assumptions in the historical shell helpers.

Benchmark results describe the tested configurations, hardware, input sizes, initialization and thread placement. Preserve archived results and use new output directories for any locally authorized rerun.

## Attribution

The [MIT license](LICENSE) covers licensed original contributions. Course materials and externally supplied implementation and helper code retain their own authorship and terms, including the CSLab/Vasileios Karakasis notices.
