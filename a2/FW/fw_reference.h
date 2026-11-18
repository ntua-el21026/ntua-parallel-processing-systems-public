#ifndef PPS_FW_REFERENCE_H
#define PPS_FW_REFERENCE_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

typedef void (*fw_solver)(int **distances, int order, int block);

/* INT_MAX is reserved for an unreachable pair. Widen the addition first so
 * composing two integer distances cannot overflow signed int arithmetic. */
static inline void fw_relax_tile(int **distances, int through,
                                 int row_start, int row_end,
                                 int col_start, int col_end)
{
    const int *pivot_row = distances[through];

    for (int row = row_start; row < row_end; ++row) {
        int *target_row = distances[row];
        int to_pivot = target_row[through];
        if (to_pivot == INT_MAX)
            continue;

        for (int col = col_start; col < col_end; ++col) {
            if (pivot_row[col] == INT_MAX)
                continue;
            int64_t alternative = (int64_t)to_pivot + pivot_row[col];
            if (alternative < target_row[col])
                target_row[col] = alternative < INT_MIN ? INT_MIN : (int)alternative;
        }
    }
}

static inline int fw_read_positive(const char *text, int *number)
{
    int value = 0;
    for (const char *digit = text; *digit != '\0'; ++digit) {
        if (*digit < '0' || *digit > '9' || value > (INT_MAX - (*digit - '0')) / 10)
            return 0;
        value = 10 * value + (*digit - '0');
    }
    if (value == 0)
        return 0;
    *number = value;
    return 1;
}

static inline int fw_run_benchmark(int argc, char **argv, const char *label,
                                  int takes_block, fw_solver solve)
{
    int order, block = 1;
    if (argc != 2 + takes_block || !fw_read_positive(argv[1], &order) ||
        (takes_block && !fw_read_positive(argv[2], &block))) {
        fprintf(stderr, "Usage: %s N%s (positive integers)\n",
                argv[0], takes_block ? " B" : "");
        return EXIT_FAILURE;
    }

    size_t width = (size_t)order;
    if (order > INT_MAX / 128 || width > SIZE_MAX / sizeof(int *) ||
        width > SIZE_MAX / sizeof(int) / width) {
        fputs("Graph dimensions exceed the representable allocation.\n", stderr);
        return EXIT_FAILURE;
    }

    int **distances = malloc(width * sizeof *distances);
    int *storage = malloc(width * width * sizeof *storage);
    if (!distances || !storage) {
        fputs("Could not allocate the graph.\n", stderr);
        free(storage);
        free(distances);
        return EXIT_FAILURE;
    }
    for (size_t row = 0; row < width; ++row)
        distances[row] = storage + row * width;
    graph_init_random(distances, -1, order, 128 * order);

    struct timespec start, finish;
    int timing_ok = clock_gettime(CLOCK_MONOTONIC, &start) == 0;
    solve(distances, order, block);
    if (clock_gettime(CLOCK_MONOTONIC, &finish) != 0 || !timing_ok) {
        fputs("Could not measure elapsed time.\n", stderr);
        free(storage);
        free(distances);
        return EXIT_FAILURE;
    }
    double elapsed = difftime(finish.tv_sec, start.tv_sec) +
        (finish.tv_nsec - start.tv_nsec) * 1e-9;

    if (takes_block)
        printf("%s,%d,%d,%.4f\n", label, order, block, elapsed);
    else
        printf("%s,%d,%.4f\n", label, order, elapsed);

    free(storage);
    free(distances);
    return EXIT_SUCCESS;
}

#endif
