/* Independent generator for the row-major host input used by CPU and CUDA. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "kmeans.h"

double *dataset_generation(int numObjs, int numCoords)
{
    if (numObjs <= 0 || numCoords <= 0 ||
        (size_t)numObjs > SIZE_MAX / sizeof(double) / (size_t)numCoords) {
        fputs("Invalid K-means dataset dimensions.\n", stderr);
        exit(EXIT_FAILURE);
    }

    size_t cells = (size_t)numObjs * (size_t)numCoords;
    double *points = (double *)malloc(cells * sizeof *points);
    if (!points) {
        fputs("Could not allocate K-means input points.\n", stderr);
        exit(EXIT_FAILURE);
    }

    /* A local multiplicative congruential stream with the minstd_rand
     * recurrence. Each call starts at the same seed for CPU/GPU validation. */
    const uint64_t modulus = 2147483647u;
    uint64_t state = 1;
    for (size_t index = 0; index < cells; ++index) {
        state = (48271u * state) % modulus;
        points[index] = (double)state / (double)modulus;
    }
    return points;
}
