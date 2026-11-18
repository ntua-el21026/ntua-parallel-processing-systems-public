# A4 · Concurrent Sorted Linked Lists

Six independently implemented sorted integer sets share the project's benchmark driver and [list API](conc_ll/ll/ll.h). The [coursework report](../docs/reports/individual/a4/) records the original comparison of workload mix, contention and thread count. The current implementations provide a separate, reproducible comparison of synchronization strategies.

| Target | Synchronization strategy |
| --- | --- |
| `x.serial` | Single-threaded pointer traversal and updates. |
| `x.cgl` | One POSIX mutex protects each complete traversal or update. |
| `x.fgl` | Hand-over-hand node locking lets operations advance through different parts of the list. |
| `x.opt` | Atomic traversal finds an adjacent pair, then local locks and a reachability check validate the pair. |
| `x.lazy` | Read-only traversal checks a removal flag. Updates lock and validate the adjacent pair, then mark removed nodes before unlinking. |
| `x.nb` | Compare-and-swap updates marked links. A competing update can unlink a logically removed node. Structural progress uses lock-free pointer-width atomics. Node allocation uses the system allocator and can block. |

The optimistic, lazy and nonblocking variants keep published nodes allocated until `ll_free`, after all workers have joined. A suspended reader can safely finish following an old link. Storage in these variants grows with the number of successful insertions over the lifetime of a list. The serial, coarse-grained and fine-grained variants reclaim a removed node immediately. All variants retain set semantics, including duplicate rejection and the full signed integer key range. Call `ll_print` for a quiescent debug listing.

The algorithm references describe the synchronization techniques. The C implementations in this directory were written independently for the project's benchmark API. [Tim Harris, *A Pragmatic Implementation of Non-Blocking Linked Lists*](https://www.microsoft.com/en-us/research/?p=149868) · [Heller et al., *A Lazy Concurrent List-Based Set Algorithm*](https://cs.brown.edu/~mph/HellerHLMSS05/2005-OPODIS-Lazy.pdf)

## Build and verify

Use GNU/Linux, a GNU C11 compiler and POSIX threads. From the repository root, run the correctness suite and build the benchmark targets.

```bash
python3 -B -m unittest discover -s tests -p test_a4_concurrent_lists.py -v
make -C a4/conc_ll
```

The suite compiles probes in temporary directories. It checks each variant against a reference set, then runs overlapping updates on the five synchronized variants. It also checks CPU affinity configuration, allocation and monotonic elapsed time. The driver defaults to one worker on the first CPU available to the process. Use `MT_CONF` to specify a comma-separated list of allowed CPU IDs. The serial variant requires one worker.

Run the following from `a4/conc_ll`. The arguments are the maximum key, then the percentages of membership checks, insertions and removals. The driver runs for ten seconds.

```bash
./x.serial 128 98 1 1
./x.nb 128 98 1 1
```

The driver signals workers through a C11 atomic flag and joins them before reading their operation counts. The [recorded benchmark files](benchmarks/) and [performance plots](diagrams/) describe the original coursework implementations and runs. New measurements belong in a separate output directory with their compiler, host, worker affinity and workload recorded.

[Course references](../EXTERNAL_INPUTS.md#course-references) identify the assignment and supporting materials. The original reports, credits and benchmark records retain their historical context. [Repository guide](../README.md)
