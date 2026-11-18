/* Independent grid utilities for the serial and distributed heat solvers. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

double max(double a, double b)
{
    return (isnan(b) || a >= b) ? a : b;
}

int converge(double **previous, double **current,
             int i_min, int i_max, int j_min, int j_max)
{
    /* An empty interior cannot contribute a disagreement to MPI_LAND. */
    if (i_min > i_max || j_min > j_max)
        return 1;
    if (!previous || !current || i_min < 0 || j_min < 0)
        return 0;

    for (size_t row = (size_t)i_min; row <= (size_t)i_max; ++row)
        for (size_t col = (size_t)j_min; col <= (size_t)j_max; ++col)
            if (!(fabs(previous[row][col] - current[row][col]) <= e))
                return 0;
    return 1;
}

double **allocate2d(int dimX, int dimY)
{
    if (dimX <= 0 || dimY <= 0 ||
        (size_t)dimX > SIZE_MAX / sizeof(double *) ||
        (size_t)dimX > SIZE_MAX / sizeof(double) / (size_t)dimY) {
        fputs("Invalid heat grid dimensions.\n", stderr);
        exit(EXIT_FAILURE);
    }

    size_t rows = (size_t)dimX, cols = (size_t)dimY;
    double **grid = malloc(rows * sizeof *grid);
    double *cells = calloc(rows * cols, sizeof *cells);
    if (!grid || !cells) {
        fputs("Could not allocate heat grid.\n", stderr);
        free(cells);
        free(grid);
        exit(EXIT_FAILURE);
    }

    /* MPI vector datatypes rely on a constant stride between rows. */
    for (size_t row = 0; row < rows; ++row)
        grid[row] = cells + row * cols;
    return grid;
}

void free2d(double **array)
{
    if (array) {
        free(array[0]);
        free(array);
    }
}

void zero2d(double **array, int dimX, int dimY)
{
    for (int row = 0; row < dimX; ++row)
        for (int col = 0; col < dimY; ++col)
            array[row][col] = 0.0;
}

void init2d(double **array, int dimX, int dimY)
{
    /* Start with a cold interior and fixed unit temperature on all edges.
     * Initialize only the logical rectangle so MPI padding stays untouched. */
    for (int row = 0; row < dimX; ++row)
        for (int col = 0; col < dimY; ++col) {
            int boundary = row == 0 || row == dimX - 1 ||
                           col == 0 || col == dimY - 1;
            array[row][col] = boundary ? val : 0.0;
        }
}

static int write_grid(FILE *stream, double **array, int dimX, int dimY)
{
    for (int row = 0; row < dimX; ++row)
        for (int col = 0; col < dimY; ++col) {
            int separator = col == dimY - 1 ? '\n' : ' ';
            if (fprintf(stream, "%.17g%c", array[row][col], separator) < 0)
                return 0;
        }
    return 1;
}

void print2d(double **array, int dimX, int dimY)
{
    if (!write_grid(stdout, array, dimX, dimY)) {
        fputs("Could not write heat grid to standard output.\n", stderr);
        exit(EXIT_FAILURE);
    }
}

void fprint2d(char *path, double **array, int dimX, int dimY)
{
    FILE *stream = fopen(path, "w");
    if (!stream) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    int written = write_grid(stream, array, dimX, dimY);
    if (fclose(stream) != 0 || !written) {
        fputs("Could not write heat grid output file.\n", stderr);
        exit(EXIT_FAILURE);
    }
}
