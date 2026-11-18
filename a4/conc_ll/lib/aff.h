/* SPDX-License-Identifier: MIT */
#ifndef LIST_BENCH_AFF_H
#define LIST_BENCH_AFF_H

/* The CPU array is borrowed until the next call in the current thread. */
void get_mtconf_options(unsigned int *workers, unsigned int **cpus);
void mt_conf_print(unsigned int workers, const unsigned int *cpus);
void setaffinity_oncpu(unsigned int cpu);

#endif
