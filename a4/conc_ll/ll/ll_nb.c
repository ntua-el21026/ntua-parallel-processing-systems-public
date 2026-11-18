/* SPDX-License-Identifier: MIT */
#include "ll_support.h"

#include <stdint.h>
#include <stdatomic.h>

typedef struct marked_node {
    int key;
    _Atomic(uintptr_t) link;
    struct marked_node *older_allocation;
} marked_node;

typedef struct {
    marked_node *previous;
    marked_node *current;
} marked_window;

struct linked_list {
    marked_node root;
    _Atomic(marked_node *) allocations;
};

#define REMOVED_BIT ((uintptr_t)1)
_Static_assert(_Alignof(marked_node) >= 2, "Node links need a free low bit");

static uintptr_t encoded(marked_node *node)
{
    return node ? (uintptr_t)node : 0;
}

static marked_node *decoded(uintptr_t link)
{
    uintptr_t address = link & ~REMOVED_BIT;

    return address ? (marked_node *)address : NULL;
}

ll_t *ll_new(void)
{
    ll_t *list = bench_alloc(sizeof(*list));

    atomic_init(&list->root.link, 0);
    atomic_init(&list->allocations, NULL);
    if (!atomic_is_lock_free(&list->root.link) || !atomic_is_lock_free(&list->allocations)) {
        fputs("The nonblocking benchmark needs lock-free pointer atomics.\n", stderr);
        free(list);
        exit(EXIT_FAILURE);
    }
    return list;
}

void ll_free(ll_t *list)
{
    marked_node *node = atomic_load(&list->allocations);

    /* Reclaim only after all workers have stopped, so published links never suffer ABA. */
    while (node) {
        marked_node *earlier = node->older_allocation;

        free(node);
        node = earlier;
    }
    free(list);
}

static marked_window live_window(ll_t *list, int key)
{
    /* A marked link describes removal of the node that contains the link. */
    for (;;) {
        marked_node *previous = &list->root;
        marked_node *current = decoded(atomic_load(&previous->link));

        while (current) {
            uintptr_t link = atomic_load(&current->link);
            marked_node *next = decoded(link);

            if (link & REMOVED_BIT) {
                uintptr_t expected = encoded(current);

                if (!atomic_compare_exchange_strong(&previous->link, &expected, encoded(next)))
                    break;
            } else {
                if (current->key >= key)
                    return (marked_window){previous, current};
                previous = current;
            }
            current = next;
        }
        if (!current)
            return (marked_window){previous, NULL};
    }
}

int ll_contains(ll_t *list, int key)
{
    marked_node *node = decoded(atomic_load(&list->root.link));

    while (node && node->key < key)
        node = decoded(atomic_load(&node->link));
    return node && node->key == key && !(atomic_load(&node->link) & REMOVED_BIT);
}

int ll_add(ll_t *list, int key)
{
    marked_node *fresh = NULL;

    for (;;) {
        marked_window window = live_window(list, key);
        uintptr_t expected = encoded(window.current);

        if (window.current && window.current->key == key) {
            free(fresh);
            return 0;
        }
        if (!fresh) {
            fresh = bench_alloc(sizeof(*fresh));
            fresh->key = key;
            atomic_init(&fresh->link, 0);
        }
        atomic_store(&fresh->link, expected);
        if (atomic_compare_exchange_strong(&window.previous->link, &expected, encoded(fresh))) {
            /* Ownership links are inspected by teardown only, after all workers join. */
            fresh->older_allocation = atomic_exchange(&list->allocations, fresh);
            return 1;
        }
    }
}

int ll_remove(ll_t *list, int key)
{
    for (;;) {
        marked_window window = live_window(list, key);
        uintptr_t next;

        if (!window.current || window.current->key != key)
            return 0;
        next = atomic_load(&window.current->link);
        if (next & REMOVED_BIT)
            return 0;
        if (atomic_compare_exchange_strong(&window.current->link, &next, next | REMOVED_BIT)) {
            uintptr_t expected = encoded(window.current);

            /* Other operations can finish the unlink if the predecessor changed. */
            atomic_compare_exchange_strong(&window.previous->link, &expected, next);
            return 1;
        }
    }
}

void ll_print(ll_t *list)
{
    bool first = true;
    marked_node *node = decoded(atomic_load(&list->root.link));

    while (node) {
        uintptr_t link = atomic_load(&node->link);

        if (!(link & REMOVED_BIT))
            print_key(node->key, &first);
        node = decoded(link);
    }
    putchar('\n');
}
