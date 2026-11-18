/* SPDX-License-Identifier: MIT */
#include "ll_support.h"

typedef struct coupled_node {
    struct coupled_node *next;
    int key;
    pthread_mutex_t mutex;
} coupled_node;

typedef struct {
    coupled_node *previous;
    coupled_node *current;
} coupled_window;

struct linked_list {
    coupled_node root;
};

ll_t *ll_new(void)
{
    ll_t *list = bench_alloc(sizeof(*list));

    list->root.next = NULL;
    checked_mutex(pthread_mutex_init(&list->root.mutex, NULL), "initialization");
    return list;
}

void ll_free(ll_t *list)
{
    coupled_node *node = list->root.next;

    while (node) {
        coupled_node *next = node->next;
        checked_mutex(pthread_mutex_destroy(&node->mutex), "destruction");
        free(node);
        node = next;
    }
    checked_mutex(pthread_mutex_destroy(&list->root.mutex), "destruction");
    free(list);
}

static coupled_window lock_window(ll_t *list, int key)
{
    coupled_node *previous = &list->root;

    checked_mutex(pthread_mutex_lock(&previous->mutex), "lock");
    while (previous->next) {
        coupled_node *current = previous->next;

        checked_mutex(pthread_mutex_lock(&current->mutex), "lock");
        if (current->key >= key)
            return (coupled_window){previous, current};
        checked_mutex(pthread_mutex_unlock(&previous->mutex), "unlock");
        previous = current;
    }
    return (coupled_window){previous, NULL};
}

static void release_window(coupled_window window)
{
    if (window.current)
        checked_mutex(pthread_mutex_unlock(&window.current->mutex), "unlock");
    checked_mutex(pthread_mutex_unlock(&window.previous->mutex), "unlock");
}

static bool window_matches(coupled_window window, int key)
{
    return window.current && window.current->key == key;
}

int ll_contains(ll_t *list, int key)
{
    coupled_window window = lock_window(list, key);
    bool found = window_matches(window, key);

    release_window(window);
    return found;
}

int ll_add(ll_t *list, int key)
{
    coupled_window window = lock_window(list, key);
    bool inserted = !window_matches(window, key);

    if (inserted) {
        coupled_node *node = bench_alloc(sizeof(*node));

        node->key = key;
        node->next = window.current;
        checked_mutex(pthread_mutex_init(&node->mutex, NULL), "initialization");
        window.previous->next = node;
    }
    release_window(window);
    return inserted;
}

int ll_remove(ll_t *list, int key)
{
    coupled_window window = lock_window(list, key);
    bool removed = window_matches(window, key);

    if (removed) {
        coupled_node *node = window.current;

        /* Holding both locks excludes every traverser that could hold this pointer. */
        window.previous->next = node->next;
        checked_mutex(pthread_mutex_unlock(&node->mutex), "unlock");
        checked_mutex(pthread_mutex_destroy(&node->mutex), "destruction");
        free(node);
    } else if (window.current) {
        checked_mutex(pthread_mutex_unlock(&window.current->mutex), "unlock");
    }
    checked_mutex(pthread_mutex_unlock(&window.previous->mutex), "unlock");
    return removed;
}

void ll_print(ll_t *list)
{
    coupled_node *previous = &list->root;
    bool first = true;

    checked_mutex(pthread_mutex_lock(&previous->mutex), "lock");
    while (previous->next) {
        coupled_node *current = previous->next;

        checked_mutex(pthread_mutex_lock(&current->mutex), "lock");
        checked_mutex(pthread_mutex_unlock(&previous->mutex), "unlock");
        print_key(current->key, &first);
        previous = current;
    }
    checked_mutex(pthread_mutex_unlock(&previous->mutex), "unlock");
    putchar('\n');
}
