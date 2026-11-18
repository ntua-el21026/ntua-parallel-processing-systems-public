/* SPDX-License-Identifier: MIT */
#include "ll_sequence.h"

struct linked_list {
    sequence_node *head;
};

ll_t *ll_new(void)
{
    ll_t *list = bench_alloc(sizeof(*list));

    list->head = NULL;
    return list;
}

void ll_free(ll_t *list)
{
    sequence_destroy(list->head);
    free(list);
}

int ll_contains(ll_t *list, int key)
{
    return sequence_matches(*sequence_slot(&list->head, key), key);
}

int ll_add(ll_t *list, int key)
{
    return sequence_insert(sequence_slot(&list->head, key), key);
}

int ll_remove(ll_t *list, int key)
{
    return sequence_erase(sequence_slot(&list->head, key), key);
}

void ll_print(ll_t *list)
{
    sequence_print(list->head);
}
