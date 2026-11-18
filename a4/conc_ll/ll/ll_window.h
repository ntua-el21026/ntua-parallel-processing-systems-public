/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_LL_WINDOW_H
#define LIST_BENCH_LL_WINDOW_H

#include "ll_support.h"

#include <stdatomic.h>

typedef struct window_node {
    int key;
    _Atomic(struct window_node *) next;
    atomic_bool removed;
    pthread_mutex_t mutex;
    struct window_node *older_allocation;
} window_node;

typedef struct {
    window_node *previous;
    window_node *current;
} node_window;

struct linked_list {
    window_node root;
    _Atomic(window_node *) allocations;
};

static inline void prepare_window_node(window_node *node, int key, window_node *next)
{
    node->key = key;
    atomic_init(&node->next, next);
    atomic_init(&node->removed, false);
    checked_mutex(pthread_mutex_init(&node->mutex, NULL), "initialization");
}

static inline ll_t *new_window_list(void)
{
    ll_t *list = bench_alloc(sizeof(*list));

    prepare_window_node(&list->root, 0, NULL);
    atomic_init(&list->allocations, NULL);
    return list;
}

static inline void free_window_list(ll_t *list)
{
    window_node *node = atomic_load(&list->allocations);

    /* The driver joins every worker before teardown, including optimistic readers. */
    while (node) {
        window_node *earlier = node->older_allocation;

        checked_mutex(pthread_mutex_destroy(&node->mutex), "destruction");
        free(node);
        node = earlier;
    }
    checked_mutex(pthread_mutex_destroy(&list->root.mutex), "destruction");
    free(list);
}

static inline node_window observe_window(ll_t *list, int key)
{
    window_node *previous = &list->root;
    window_node *current = atomic_load(&previous->next);

    while (current && current->key < key) {
        previous = current;
        current = atomic_load(&current->next);
    }
    return (node_window){previous, current};
}

static inline void lock_observed_window(node_window window)
{
    checked_mutex(pthread_mutex_lock(&window.previous->mutex), "lock");
    if (window.current)
        checked_mutex(pthread_mutex_lock(&window.current->mutex), "lock");
}

static inline void unlock_observed_window(node_window window)
{
    if (window.current)
        checked_mutex(pthread_mutex_unlock(&window.current->mutex), "unlock");
    checked_mutex(pthread_mutex_unlock(&window.previous->mutex), "unlock");
}

static inline bool observed_key(node_window window, int key)
{
    return window.current && window.current->key == key;
}

static inline void insert_at_window(ll_t *list, node_window window, int key)
{
    window_node *node = bench_alloc(sizeof(*node));

    prepare_window_node(node, key, window.current);
    /* Only quiescent teardown reads the ownership links. */
    node->older_allocation = atomic_exchange(&list->allocations, node);
    atomic_store(&window.previous->next, node);
}

static inline void unlink_window(node_window window)
{
    atomic_store(&window.previous->next, atomic_load(&window.current->next));
}

static inline void print_window_list(ll_t *list)
{
    bool first = true;

    for (window_node *node = atomic_load(&list->root.next); node;
         node = atomic_load(&node->next)) {
        if (!atomic_load(&node->removed))
            print_key(node->key, &first);
    }
    putchar('\n');
}

#endif
