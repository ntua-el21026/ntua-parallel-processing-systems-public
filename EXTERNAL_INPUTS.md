# Authorized local course inputs

The original team work, measured timings, plans and reports are preserved. Existing MIT terms apply to original contributions. CSLab material and the CSLab/Vasileios Karakasis helpers retain their separate authorship and terms.

## Material roles and retained evidence

| External group | Role and authorized setup | Retained evidence and reproduction |
| --- | --- | --- |
| 21 course statements, lecture slides, and lab guides | Task definitions and background on OpenMP, synchronization, GPU and MPI. Consult the exact edition through the NTUA entry points below. | Full report and figure inspection. The original reading depends on course access. |
| Game of Life serial baseline, MPI skeleton, and base K-means sources | Reference source needed to reconstitute the recorded mixed implementations. Supply one matching copy of each base to the hash-checked local assembly tool. | Student changes, six validated reconstructions and historic timings. Execution is conditional on local inputs and toolchains. |
| K-means drivers, file I/O, utilities, serial references and the named CUDA allocation/error helpers | Support code and serial comparison paths for the Makefile targets. Obtain the exact release via the course resources and use the locations below. | Retained kernels and original comparative measurements. Full target builds are conditional on companion files, OpenMP, CUDA or MPI as appropriate. |
| Selected compiled objects and executables | Original build products tied to external source or libraries. Rebuild locally after obtaining the associated source and dependencies. | Thirteen independently scoped objects and archived logs remain available. |
| Historical starter notes and code printouts | Older course instructions and inline/composite source listings used to explain submissions. The MPI skeleton is an assembly input; the older lab note is teaching context. | Original report narrative and measurements remain, with scoped replacement pages that identify the sources. |

## Course references

Access the exact 2025–26 edition through the authorized course resources. The public CSLab index also identifies related editions. [NTUA course](https://www.ece.ntua.gr/en/education/undergraduate/courses?courseId=3257), [CSLab course materials](https://legacy.cslab.ece.ntua.gr/courses/pps/assign.go)

- `a1/docs/askisi1-2025-2026.pdf`
- `a1/docs/pps_exercises_2025_2026.pdf`
- `a2/docs/askisi2-2025-26.pdf`
- `a2/docs/pps_exercises_2025_2026.pdf`
- `a3/docs/pps_exercises_2025_2026.pdf`
- `a4/docs/pps_exercises_2025_2026.pdf`
- `a5/docs/pps_exercises_2025_2026.pdf`
- `a6/docs/askisi4-2025-2026.pdf`
- `a6/docs/pps_exercises_2025_2026.pdf`
- `docs/lecture_slides/MPI.pdf`
- `docs/lecture_slides/OpenMP.pdf`
- `docs/lecture_slides/Δίκτυα Διασύνδεσης.pdf`
- `docs/lecture_slides/Εισαγωγή.pdf`
- `docs/lecture_slides/Παράλληλος Προγραμματισμός σε Επεξεργαστές Γραφικών.pdf`
- `docs/lecture_slides/Παράλληλος Προγραμματισμός, Μέρος Α.pdf`
- `docs/lecture_slides/Παράλληλος Προγραμματισμός, Μέρος Β.pdf`
- `docs/lecture_slides/Παράλληλος Προγραμματισμός, Μέρος Γ.pdf`
- `docs/lecture_slides/Παράλληλος Προγραμματισμός, Μέρος Δ.pdf`
- `docs/lecture_slides/Συγχρονισμός.pdf`
- `docs/misc/pps-lab-guide_pres.pdf`
- `docs/misc/pps_lab_guide.pdf`

## Local setup and builds

Assemble the Game of Life, MPI heat and MPI K-means changes with the [local assembly tools](course-source-reconstruction/README.md). Provide the files below at their original relative paths in the local working copy before invoking the original Makefiles. Keep supplied course files out of a redistributed snapshot. Compare rebuilt outputs with the historical reports using the original configurations. Use compatible CUDA and MPI toolchains, authorized helper sources, and the applicable cluster environment for the corresponding targets.

- `a1/game_of_life.c`
- `a2/kmeans/file_io.c`
- `a2/kmeans/main.c`
- `a2/kmeans/seq_kmeans.c`
- `a2/kmeans/util.c`
- `a3/file_io.c`
- `a3/main.c`
- `a3/seq_kmeans.c`
- `a3/util.c`
- `a5/Helper_functions/alloc.c`
- `a5/Helper_functions/alloc.h`
- `a5/Helper_functions/decls.h`
- `a5/Helper_functions/error.c`
- `a5/Helper_functions/util.c`
- `a6/heat_transfer/serial/GaussSeidelSOR_serial.c`
- `a6/heat_transfer/serial/Jacobi_serial.c`
- `a6/heat_transfer/serial/RedBlackSOR_serial.c`
- `a6/kmeans/util.c`

Three separately compiled OpenMP objects, two Assignment 2 OpenMP objects, seven lock objects and the original MPI K-means implementation object remain available. Six of the lock objects were rebuilt against the [independent A3 allocation header](a3/README.md#build-and-validation). Build linked executables locally after supplying the matching dependencies. The appendix pages reference the corresponding source assembly records. The original reports and measurements remain available for interpreting the historical results.

<a id="retained-source-provenance"></a>

## Independent implementations

These components implement the required interfaces from the assignment descriptions, the calling code, and standard algorithms and system APIs. Their paths make them available to the existing builds. The original course implementations are historical inputs outside this version of the repository. The student parallel kernels, reports, and measurements retain their original provenance.

| Paths | Role |
| --- | --- |
| `a2/FW/fw.c`, `a2/FW/fw_sr.c`, `a2/FW/fw_tiled.c` | Serial shortest-path references with a direct recurrence, recursive blocking and tiling. See the [Floyd–Warshall build and tests](a2/README.md#local-builds). |
| `a3/locks/alloc.h` | Checked allocation and deallocation for the student lock variants. The six allocator-backed objects can be built without the course K-means driver. |
| `a4/conc_ll/ll/ll_serial.c`, `ll_cgl.c`, `ll_fgl.c`, `ll_opt.c`, `ll_lazy.c`, `ll_nb.c` in the same directory | Integer sorted sets using serial execution, coarse- or fine-grained mutexes, optimistic or lazy synchronization, and nonblocking updates. See the [benchmark and validation guide](a4/README.md). |
| `a4/conc_ll/lib/aff.c`, `aff.h`, `alloc.h`, `timer.h` in the same directory | Linux thread affinity, allocator interfaces, and monotonic wall-clock timing. |
| `a5/Helper_functions/file_io.c` | Host-side generation of reproducible synthetic points through the existing K-means interface. The remaining CUDA support dependencies are listed above. |
| `a6/heat_transfer/mpi/utils.c`, `a6/heat_transfer/serial/utils.c` | Contiguous grids, boundary initialization, convergence checks and output helpers used by the serial and MPI drivers. |

The project MIT license applies to independently authored project code. Externally acquired CSLab and Vasileios Karakasis components retain their own authorship and terms. Use an authorized historical environment when comparing timings in the submitted reports. Treat measurements from the independent sources as new runs.
