/* SPDX-License-Identifier: MIT */
#include "ll_window.h"

ll_t *ll_new(void)
{
    return new_window_list();
}

void ll_free(ll_t *list)
{
    free_window_list(list);
}

static node_window lock_reachable_window(ll_t *list, int key)
{
    for (;;) {
        node_window window = observe_window(list, key);
        window_node *node = &list->root;

        lock_observed_window(window);
        while (node && node != window.previous)
            node = atomic_load(&node->next);
        if (node && atomic_load(&node->next) == window.current)
            return window;
        unlock_observed_window(window);
    }
}

int ll_contains(ll_t *list, int key)
{
    node_window window = lock_reachable_window(list, key);
    bool found = observed_key(window, key);

    unlock_observed_window(window);
    return found;
}

int ll_add(ll_t *list, int key)
{
    node_window window = lock_reachable_window(list, key);
    bool inserted = !observed_key(window, key);

    if (inserted)
        insert_at_window(list, window, key);
    unlock_observed_window(window);
    return inserted;
}

int ll_remove(ll_t *list, int key)
{
    node_window window = lock_reachable_window(list, key);
    bool removed = observed_key(window, key);

    if (removed)
        unlink_window(window);
    unlock_observed_window(window);
    return removed;
}

void ll_print(ll_t *list)
{
    print_window_list(list);
}
