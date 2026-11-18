/* SPDX-License-Identifier: MIT */
#include "ll/ll.h"

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>

enum { WORKERS = 6, ROUNDS = 128, STRIDE = 60, DISJOINT_ROUNDS = 12 };

typedef struct {
    ll_t *list;
    pthread_barrier_t *barrier;
    int index;
    int adds[ROUNDS];
    int removes[ROUNDS];
} worker_input;

static void rendezvous(pthread_barrier_t *barrier)
{
    int result = pthread_barrier_wait(barrier);

    assert(result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD);
}

static void sequential(void)
{
    ll_t *list = ll_new();
    int members[65] = {0};
    uint32_t random = 431;
    ll_t *other = ll_new();

    assert(ll_add(list, INT_MIN));
    assert(ll_add(list, INT_MAX));
    assert(ll_contains(list, INT_MIN));
    assert(ll_contains(list, INT_MAX));
    assert(!ll_contains(other, INT_MIN));
    assert(!ll_add(list, INT_MIN));
    assert(ll_remove(list, INT_MIN));
    assert(!ll_remove(list, INT_MIN));
    assert(ll_remove(list, INT_MAX));
    ll_free(other);

    for (int step = 0; step < 5000; ++step) {
        unsigned int slot, operation;
        int key;

        random = UINT32_C(1664525) * random + UINT32_C(1013904223);
        slot = random % 65;
        key = (int)slot - 32;
        operation = (random >> 16) % 3;
        switch (operation) {
        case 0:
            assert(ll_contains(list, key) == members[slot]);
            break;
        case 1:
            assert(ll_add(list, key) == !members[slot]);
            members[slot] = 1;
            break;
        default:
            assert(ll_remove(list, key) == members[slot]);
            members[slot] = 0;
            break;
        }
    }
    for (int index = 0; index < 65; ++index) {
        assert(ll_contains(list, index - 32) == members[index]);
        assert(ll_remove(list, index - 32) == members[index]);
    }

    assert(ll_add(list, 20));
    assert(ll_add(list, -1));
    assert(ll_add(list, 8));
    ll_print(list);
    ll_free(list);
}

static void *contend_for_key(void *argument)
{
    worker_input *worker = argument;

    for (int round = 0; round < ROUNDS; ++round) {
        int key = round % 13 - 6;

        rendezvous(worker->barrier);
        worker->adds[round] = ll_add(worker->list, key);
        rendezvous(worker->barrier);
        assert(ll_contains(worker->list, key));
        rendezvous(worker->barrier);
        worker->removes[round] = ll_remove(worker->list, key);
        rendezvous(worker->barrier);
        assert(!ll_contains(worker->list, key));
        rendezvous(worker->barrier);
    }
    return NULL;
}

static void *mutate_other_ranges(void *argument)
{
    worker_input *worker = argument;

    rendezvous(worker->barrier);
    for (int round = 0; round < DISJOINT_ROUNDS; ++round) {
        /* Interleaved key ranges keep traversals and predecessor updates overlapping. */
        for (int key = worker->index; key < STRIDE * WORKERS; key += WORKERS) {
            assert(ll_add(worker->list, key));
            assert(ll_contains(worker->list, key));
            assert(!ll_add(worker->list, key));
        }
        for (int key = worker->index; key < STRIDE * WORKERS; key += WORKERS) {
            if (round == DISJOINT_ROUNDS - 1 && (key / WORKERS) % 2)
                continue;
            assert(ll_remove(worker->list, key));
            assert(!ll_contains(worker->list, key));
        }
    }
    return NULL;
}

static void concurrent(int competing)
{
    ll_t *list = ll_new();
    pthread_barrier_t barrier;
    pthread_t threads[WORKERS];
    worker_input input[WORKERS] = {0};

    assert(!pthread_barrier_init(&barrier, NULL, WORKERS));
    for (int worker = 0; worker < WORKERS; ++worker) {
        input[worker].list = list;
        input[worker].barrier = &barrier;
        input[worker].index = worker;
        assert(!pthread_create(&threads[worker], NULL,
            competing ? contend_for_key : mutate_other_ranges, &input[worker]));
    }
    for (int worker = 0; worker < WORKERS; ++worker)
        assert(!pthread_join(threads[worker], NULL));
    assert(!pthread_barrier_destroy(&barrier));

    if (competing) {
        for (int round = 0; round < ROUNDS; ++round) {
            int added = 0, removed = 0;

            for (int worker = 0; worker < WORKERS; ++worker) {
                added += input[worker].adds[round];
                removed += input[worker].removes[round];
            }
            assert(added == 1);
            assert(removed == 1);
        }
    } else {
        for (int key = 0; key < STRIDE * WORKERS; ++key)
            assert(ll_contains(list, key) == (key / WORKERS) % 2);
    }
    ll_free(list);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (!strcmp(argv[1], "sequential")) {
        sequential();
        return 0;
    }
    assert(!strcmp(argv[1], "competing") || !strcmp(argv[1], "disjoint"));
    concurrent(!strcmp(argv[1], "competing"));
    return 0;
}
