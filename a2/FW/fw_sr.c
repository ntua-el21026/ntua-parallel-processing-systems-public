/* Independent serial recursive traversal of the Floyd-Warshall update. */
#include "fw_reference.h"

/* Each pivot is completed before the next pivot begins. Within one pivot
 * its own row and column are stable when the pivot diagonal is zero. Split
 * the destination rectangle along its longer dimension for cache locality. */
static void visit(int **distances, int through, int first_row, int last_row,
                  int first_col, int last_col, int leaf)
{
    int rows = last_row - first_row;
    int cols = last_col - first_col;
    if (rows <= leaf && cols <= leaf) {
        fw_relax_tile(distances, through, first_row, last_row, first_col, last_col);
    } else if (rows >= cols) {
        int middle = first_row + rows / 2;
        visit(distances, through, first_row, middle, first_col, last_col, leaf);
        visit(distances, through, middle, last_row, first_col, last_col, leaf);
    } else {
        int middle = first_col + cols / 2;
        visit(distances, through, first_row, last_row, first_col, middle, leaf);
        visit(distances, through, first_row, last_row, middle, last_col, leaf);
    }
}

void fw_recursive(int **distances, int order, int leaf)
{
    for (int through = 0; through < order; ++through)
        visit(distances, through, 0, order, 0, order, leaf);
}

#ifndef FW_REFERENCE_NO_MAIN
int main(int argc, char **argv)
{
    return fw_run_benchmark(argc, argv, "FW_SR", 1, fw_recursive);
}
#endif
