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

static node_window lock_live_window(ll_t *list, int key)
{
    for (;;) {
        node_window window = observe_window(list, key);

        lock_observed_window(window);
        if (!atomic_load(&window.previous->removed)
            && atomic_load(&window.previous->next) == window.current
            && (!window.current || !atomic_load(&window.current->removed)))
            return window;
        unlock_observed_window(window);
    }
}

int ll_contains(ll_t *list, int key)
{
    node_window window = observe_window(list, key);

    return observed_key(window, key) && !atomic_load(&window.current->removed);
}

int ll_add(ll_t *list, int key)
{
    node_window window = lock_live_window(list, key);
    bool inserted = !observed_key(window, key);

    if (inserted)
        insert_at_window(list, window, key);
    unlock_observed_window(window);
    return inserted;
}

int ll_remove(ll_t *list, int key)
{
    node_window window = lock_live_window(list, key);
    bool removed = observed_key(window, key);

    if (removed) {
        atomic_store(&window.current->removed, true);
        unlink_window(window);
    }
    unlock_observed_window(window);
    return removed;
}

void ll_print(ll_t *list)
{
    print_window_list(list);
}
