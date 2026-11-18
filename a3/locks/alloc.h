#ifndef A3_LOCK_ALLOC_H
#define A3_LOCK_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline void *a3_allocate_elements(size_t count, size_t element_size)
{
    if (element_size != 0 && count > SIZE_MAX / element_size) {
        fputs("A3 lock allocation exceeds the size limit\n", stderr);
        exit(EXIT_FAILURE);
    }

    size_t bytes = count * element_size;
    void *storage = malloc(bytes ? bytes : 1);
    if (storage == NULL) {
        fputs("A3 lock allocation failed\n", stderr);
        exit(EXIT_FAILURE);
    }
    return storage;
}

/* Use the destination's element type, including for arrays and pointer members. */
#define XMALLOC(destination, count) \
    ((destination) = a3_allocate_elements((size_t)(count), sizeof *(destination)))

#define XFREE(pointer) free(pointer)

#endif
