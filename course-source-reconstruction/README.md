# Local course-source assembly

The components below combine the original course base and changes recorded in the coursework. The overlay preserves changed lines and references unchanged lines by location. It does not include the course base. The original authorship and source attributions remain applicable to locally assembled sources.

Obtain the course edition through an authorized course account or source. [ntua-parallel-processing-systems](https://www.ece.ntua.gr/en/education/undergraduate/courses?courseId=3257)

Place each input in an external directory under the relative path in the manifest. Run `python3 course-source-reconstruction/restore.py /path/to/authorized-bases /path/to/local-working-copy` from the project root. The script verifies both the source and reconstructed bytes and refuses existing outputs. Keep locally assembled sources and licensed inputs outside any redistributed snapshot.

| Local input path | Source | Required SHA-256 |
|---|---|---|
| `a1/game_of_life.c` | Matched serial version from the course assignment 2025–26 | `0af409474886817c2420a22fbc8b6a222aeb036a11db82a11422ff782c63a122` |
| `a6/heat_transfer/mpi/mpi_skeleton.c` | One input shared by the three MPI heat-transfer outputs, recorded in original commit b2aeb14 | `a7ad1ae47b832edf0919b8892e250b3550742f1172158d4f0e5811103cad3613` |
| `a6/kmeans/file_io.c` | Original import at b2aeb14 | `04060c6d2dd828050a62f91b86c631ec00218458bcb9b770cd8c2aa655dda5fa` |
| `a6/kmeans/main.c` | Original import at b2aeb14 | `b9f9ea9b2f09fb2d8000afdc985a8441a19ce66efe1c8b95dd608df05d49f5d7` |

The [external-input register](../EXTERNAL_INPUTS.md) lists the original base sources, companion headers and helper libraries to obtain separately.
