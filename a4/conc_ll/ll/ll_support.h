/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_LL_SUPPORT_H
#define LIST_BENCH_LL_SUPPORT_H

#include "ll.h"
#include "../lib/alloc.h"

#include <pthread.h>
#include <stdbool.h>

static inline void checked_mutex(int error, const char *action)
{
    if (error) {
        fprintf(stderr, "Linked-list mutex %s failed (error %d).\n", action, error);
        exit(EXIT_FAILURE);
    }
}

static inline void print_key(int key, bool *first)
{
    printf("%s%d", *first ? "" : " ", key);
    *first = false;
}

#endif
