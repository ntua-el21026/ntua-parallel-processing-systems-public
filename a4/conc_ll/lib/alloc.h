/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_ALLOC_H
#define LIST_BENCH_ALLOC_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static inline void *bench_alloc(size_t bytes)
{
    void *memory = malloc(bytes);

    if (!memory) {
        fputs("Could not allocate memory for the linked-list benchmark.\n", stderr);
        exit(EXIT_FAILURE);
    }
    return memory;
}

#endif
