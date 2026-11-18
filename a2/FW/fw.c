/* Independent serial all-pairs shortest-path reference. */
#include "fw_reference.h"

void fw_plain(int **distances, int order, int unused_block)
{
    (void)unused_block;
    for (int through = 0; through < order; ++through)
        fw_relax_tile(distances, through, 0, order, 0, order);
}

#ifndef FW_REFERENCE_NO_MAIN
int main(int argc, char **argv)
{
    return fw_run_benchmark(argc, argv, "FW", 0, fw_plain);
}
#endif
