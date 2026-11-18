/* SPDX-License-Identifier: MIT */
#include "ll_sequence.h"

struct linked_list {
    sequence_node *head;
    pthread_mutex_t mutex;
};

ll_t *ll_new(void)
{
    ll_t *list = bench_alloc(sizeof(*list));

    list->head = NULL;
    checked_mutex(pthread_mutex_init(&list->mutex, NULL), "initialization");
    return list;
}

void ll_free(ll_t *list)
{
    sequence_destroy(list->head);
    checked_mutex(pthread_mutex_destroy(&list->mutex), "destruction");
    free(list);
}

int ll_contains(ll_t *list, int key)
{
    int found;

    checked_mutex(pthread_mutex_lock(&list->mutex), "lock");
    found = sequence_matches(*sequence_slot(&list->head, key), key);
    checked_mutex(pthread_mutex_unlock(&list->mutex), "unlock");
    return found;
}

int ll_add(ll_t *list, int key)
{
    int inserted;

    checked_mutex(pthread_mutex_lock(&list->mutex), "lock");
    inserted = sequence_insert(sequence_slot(&list->head, key), key);
    checked_mutex(pthread_mutex_unlock(&list->mutex), "unlock");
    return inserted;
}

int ll_remove(ll_t *list, int key)
{
    int removed;

    checked_mutex(pthread_mutex_lock(&list->mutex), "lock");
    removed = sequence_erase(sequence_slot(&list->head, key), key);
    checked_mutex(pthread_mutex_unlock(&list->mutex), "unlock");
    return removed;
}

void ll_print(ll_t *list)
{
    checked_mutex(pthread_mutex_lock(&list->mutex), "lock");
    sequence_print(list->head);
    checked_mutex(pthread_mutex_unlock(&list->mutex), "unlock");
}
