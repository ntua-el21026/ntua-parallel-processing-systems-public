/* SPDX-License-Identifier: MIT */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "lib/aff.h"
#include "lib/alloc.h"
#include "lib/timer.h"

#include <assert.h>
#include <sched.h>

int main(void)
{
    unsigned int count, *cpus;
    timer_tt *timer = timer_init();
    struct timespec pause = {.tv_nsec = 3000000};
    cpu_set_t actual;
    void *scratch = bench_alloc(32);

    free(scratch);
    get_mtconf_options(&count, &cpus);
    mt_conf_print(count, cpus);
    assert(count > 0);
    setaffinity_oncpu(cpus[0]);
    assert(!sched_getaffinity(0, sizeof(actual), &actual));
    assert(CPU_COUNT(&actual) == 1 && CPU_ISSET(cpus[0], &actual));

    timer_start(timer);
    assert(timer_report_sec(timer) == 0.0);
    assert(!nanosleep(&pause, NULL));
    timer_stop(timer);
    assert(timer_report_sec(timer) > 0.0);
    return 0;
}
