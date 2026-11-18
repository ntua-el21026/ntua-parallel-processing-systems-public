/* Independent blocked Floyd-Warshall reference. */
#include "fw_reference.h"

static int tile_end(int start, int order, int block)
{
    return order - start <= block ? order : start + block;
}

static void update(int **distances, int pivot, int pivot_end,
                   int row, int row_end, int col, int col_end)
{
    for (int through = pivot; through < pivot_end; ++through)
        fw_relax_tile(distances, through, row, row_end, col, col_end);
}

void fw_tiled(int **distances, int order, int block)
{
    for (int pivot = 0; pivot < order; pivot = tile_end(pivot, order, block)) {
        int pivot_end = tile_end(pivot, order, block);

        /* Close the diagonal first, then update its row and column. */
        update(distances, pivot, pivot_end, pivot, pivot_end, pivot, pivot_end);
        for (int other = 0; other < order; other = tile_end(other, order, block)) {
            if (other == pivot)
                continue;
            int end = tile_end(other, order, block);
            update(distances, pivot, pivot_end, pivot, pivot_end, other, end);
            update(distances, pivot, pivot_end, other, end, pivot, pivot_end);
        }

        /* All remaining tiles now have both completed input stripes. */
        for (int row = 0; row < order; row = tile_end(row, order, block)) {
            if (row == pivot)
                continue;
            for (int col = 0; col < order; col = tile_end(col, order, block)) {
                if (col != pivot)
                    update(distances, pivot, pivot_end, row, tile_end(row, order, block),
                           col, tile_end(col, order, block));
            }
        }
    }
}

#ifndef FW_REFERENCE_NO_MAIN
int main(int argc, char **argv)
{
    return fw_run_benchmark(argc, argv, "FW_TILED", 1, fw_tiled);
}
#endif
