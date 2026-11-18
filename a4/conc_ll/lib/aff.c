/* SPDX-License-Identifier: MIT */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "aff.h"

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The benchmark driver reserves 128 thread handles. */
enum { BENCH_MAX_WORKERS = 128 };

static const char *skip_space(const char *text)
{
    while (isspace((unsigned char)*text))
        ++text;
    return text;
}

static void invalid_config(void)
{
    fputs("MT_CONF expects up to 128 allowed CPU IDs separated by commas.\n", stderr);
    exit(EXIT_FAILURE);
}

void get_mtconf_options(unsigned int *workers, unsigned int **cpus)
{
    static _Thread_local unsigned int cpu_ids[BENCH_MAX_WORKERS];
    const char *config = getenv("MT_CONF");
    cpu_set_t allowed;
    unsigned int count = 0;

    if (sched_getaffinity(0, sizeof(allowed), &allowed) == -1) {
        perror("sched_getaffinity");
        exit(EXIT_FAILURE);
    }

    if (!config || !*skip_space(config)) {
        for (unsigned int cpu = 0; cpu < CPU_SETSIZE; ++cpu) {
            if (CPU_ISSET(cpu, &allowed)) {
                cpu_ids[count++] = cpu;
                break;
            }
        }
    } else {
        do {
            char *end;
            unsigned long cpu;

            config = skip_space(config);
            if (count == BENCH_MAX_WORKERS || !isdigit((unsigned char)*config))
                invalid_config();
            errno = 0;
            cpu = strtoul(config, &end, 10);
            if (errno || cpu >= CPU_SETSIZE || !CPU_ISSET(cpu, &allowed))
                invalid_config();
            cpu_ids[count++] = (unsigned int)cpu;
            config = skip_space(end);
            if (!*config)
                break;
            if (*config++ != ',')
                invalid_config();
        } while (1);
    }

    if (!count)
        invalid_config();
    *workers = count;
    *cpus = cpu_ids;
}

void mt_conf_print(unsigned int workers, const unsigned int *cpus)
{
    fputs("MT_CONF=", stdout);
    for (unsigned int index = 0; index < workers; ++index)
        printf("%s%u", index ? "," : "", cpus[index]);
    putchar('\n');
}

void setaffinity_oncpu(unsigned int cpu)
{
    cpu_set_t affinity;
    int error;

    if (cpu >= CPU_SETSIZE)
        invalid_config();
    CPU_ZERO(&affinity);
    CPU_SET(cpu, &affinity);
    error = pthread_setaffinity_np(pthread_self(), sizeof(affinity), &affinity);
    if (error) {
        fprintf(stderr, "Could not set CPU affinity: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}
