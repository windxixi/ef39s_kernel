/*
 * drivers/cpufreq/cpufreq_perflock.h
 * 
 * Copyright (C) 2010 Jae Woo Jeon
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public license version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _LINUX_CPUFREQ_PERFLOCK_H
#define _LINUX_CPUFREQ_PREFLOCK_H

#include <linux/list.h>

enum {
  PERF_LOCK_UNLOCKED  = 0,
  PERF_LOCK_LOCKED    = 1,
};

struct perf_lock {
  struct list_head  link;
  unsigned int      flags;
  const char*       name;
};

#ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE

/* Defined in cpufreq.c */
extern int cpufreq_perflock_set_governor(struct cpufreq_policy* policy, 
                                         const char* buf);
/* Defined in cpufreq_ondemand.c */
extern unsigned int cpufreq_ondemand_get_sampling_rate(void);
/* Defined in cpufreq_ondemand.c */
extern void cpufreq_ondemand_set_sampling_rate(unsigned int sampling_rate);

extern ssize_t cpufreq_perflock_show_perf_lock(struct cpufreq_policy* policy, char* buf);
extern ssize_t cpufreq_perflock_show_perf_unlock(struct cpufreq_policy* policy, char* buf);
extern ssize_t cpufreq_perflock_store_perf_lock(struct cpufreq_policy* policy, const char* buf, size_t count);
extern ssize_t cpufreq_perflock_store_perf_unlock(struct cpufreq_policy* policy, const char* buf, size_t count);
extern void    cpufreq_perflock_set_ondemand_sampling_rate(unsigned int sampling_rate);

#endif /* #ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE */

#endif /* _LINUX_CPUFREQ_PERFLOCK_H */
