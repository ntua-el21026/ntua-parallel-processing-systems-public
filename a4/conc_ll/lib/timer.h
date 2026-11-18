/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_TIMER_H
#define LIST_BENCH_TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    struct timespec started;
    struct timespec stopped;
} timer_tt;

/* The driver uses one stopwatch per calling thread. */
static inline timer_tt *timer_init(void)
{
    static _Thread_local timer_tt timer;
    timer = (timer_tt){0};
    return &timer;
}

static inline void bench_read_time(struct timespec *reading)
{
    if (clock_gettime(CLOCK_MONOTONIC, reading) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
}

static inline void timer_start(timer_tt *timer)
{
    bench_read_time(&timer->started);
    timer->stopped = timer->started;
}

static inline void timer_stop(timer_tt *timer)
{
    bench_read_time(&timer->stopped);
}

static inline double timer_report_sec(const timer_tt *timer)
{
    return difftime(timer->stopped.tv_sec, timer->started.tv_sec)
        + 1e-9 * (double)(timer->stopped.tv_nsec - timer->started.tv_nsec);
}

#endif
