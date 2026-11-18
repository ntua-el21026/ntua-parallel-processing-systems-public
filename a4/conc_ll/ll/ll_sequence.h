/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_LL_SEQUENCE_H
#define LIST_BENCH_LL_SEQUENCE_H

#include "ll_support.h"

typedef struct sequence_node {
    struct sequence_node *next;
    int key;
} sequence_node;

/* Returning the link, rather than just a node, also handles front insertion. */
static inline sequence_node **sequence_slot(sequence_node **link, int key)
{
    while (*link && (*link)->key < key)
        link = &(*link)->next;
    return link;
}

static inline bool sequence_matches(sequence_node *node, int key)
{
    return node && node->key == key;
}

static inline int sequence_insert(sequence_node **link, int key)
{
    sequence_node *node;

    if (sequence_matches(*link, key))
        return 0;
    node = bench_alloc(sizeof(*node));
    *node = (sequence_node){.next = *link, .key = key};
    *link = node;
    return 1;
}

static inline int sequence_erase(sequence_node **link, int key)
{
    sequence_node *removed = *link;

    if (!sequence_matches(removed, key))
        return 0;
    *link = removed->next;
    free(removed);
    return 1;
}

static inline void sequence_destroy(sequence_node *head)
{
    while (head) {
        sequence_node *next = head->next;
        free(head);
        head = next;
    }
}

static inline void sequence_print(sequence_node *head)
{
    bool first = true;

    for (sequence_node *node = head; node; node = node->next)
        print_key(node->key, &first);
    putchar('\n');
}

#endif
