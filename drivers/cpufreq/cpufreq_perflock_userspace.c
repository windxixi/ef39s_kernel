/*
 * drivers/cpufreq/cpufreq_perflock_userspace.c
 * 
 * Copyright (C) 2010 Jae Woo Jeon
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/rbtree.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/slab.h>

#include "cpufreq_perflock.h"

enum {
	DEBUG_FAILURE	= BIT(0),
	DEBUG_ERROR	= BIT(1),
	DEBUG_NEW	= BIT(2),
	DEBUG_ACCESS	= BIT(3),
	DEBUG_LOOKUP	= BIT(4),
};
static int debug_mask = DEBUG_FAILURE;
module_param_named(debug_mask, debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);
  
#ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE

static DEFINE_MUTEX(tree_lock);

#define MUTEX_LOCK()    mutex_lock(&tree_lock)
#define MUTEX_UNLOCK()  mutex_unlock(&tree_lock)

#define DEF_ONDEMAND_SAMPLING_RATE 500000

static struct ondemand_params_s {
  unsigned int sampling_rate;
} ondemand_params = {
  .sampling_rate = DEF_ONDEMAND_SAMPLING_RATE
};

struct user_perf_lock {
  struct rb_node    node;
  struct perf_lock  perf_lock;
  char              name[0];
};
struct rb_root user_perf_locks;

static unsigned int user_perf_lock_count = 0;

#endif /* #ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE */

static int perf_lock_active(struct perf_lock* lock)
{
  return lock->flags == PERF_LOCK_LOCKED;
}

#ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE

void cpufreq_perflock_set_ondemand_sampling_rate(unsigned int sampling_rate)
{
  if (debug_mask & DEBUG_ACCESS)
    pr_info("cpufreq_perflock_set_ondemand_sampling_rate: %d\n", sampling_rate);
  ondemand_params.sampling_rate = sampling_rate;
}

static struct user_perf_lock* lookup_perf_lock_name(const char* buf, int allocate)
{
  struct rb_node**       p = &user_perf_locks.rb_node;
  struct rb_node*        parent = NULL;
  struct user_perf_lock* l = NULL;
  int                    diff;
  int                    name_len = 0;
  const char*            arg = buf;
  
  while (*arg && !isspace(*arg))
  {
    ++name_len;
    ++arg;
  }
  
  while (*p) {
    parent = *p;
    l = rb_entry(parent, struct user_perf_lock, node);
    diff = strncmp(buf, l->name, name_len);
    if (!diff && l->name[name_len])
      diff = -1;
    if (debug_mask & DEBUG_ERROR)
      pr_info("lookup_perf_lock_name: compare %.*s %s %d\n",
        name_len, buf, l->name, diff);
      
    if (diff < 0)
    {
      p = &(*p)->rb_left;
    }
    else if (diff > 0)
    {
      p = &(*p)->rb_right;
    }
    else
      return l;
  }
  
  if (!allocate)
  {
    if (debug_mask & DEBUG_ERROR)
      pr_info("lookup_perf_lock_name: %.*s not found\n",
        name_len, buf);
    return ERR_PTR(-EINVAL);
  }
  
  l = kzalloc(sizeof(*l) + name_len + 1, GFP_KERNEL);
  if (NULL == l)
  {
    if (debug_mask & DEBUG_FAILURE)
      pr_err("lookup_perf_lock_name: failed to allocate memory for %.*s\n",
        name_len, buf);
    return ERR_PTR(-ENOMEM);
  }
  memcpy(l->name, buf, name_len);
  l->name[name_len] = 0;
  if (debug_mask & DEBUG_ERROR)
    pr_info("lookup_perf_lock_name: new perf lock '%s'\n", l->name);
  rb_link_node(&l->node, parent, p);
  rb_insert_color(&l->node, &user_perf_locks);
  
  return l;
}

ssize_t cpufreq_perflock_show_perf_lock(struct cpufreq_policy* policy, char* buf)
{
  char* s = buf;
  char* end = buf + PAGE_SIZE;
  struct rb_node* n;
  
  MUTEX_LOCK();
  
  for (n = rb_first(&user_perf_locks); n != NULL; n = rb_next(n))
  {
    struct user_perf_lock* l = rb_entry(n, struct user_perf_lock, node);
    if (perf_lock_active(&l->perf_lock))
    {
      s += snprintf(s, end - s, "%s ", l->name);
    }
  }
  s += snprintf(s, end - s, "\n");
  
  MUTEX_UNLOCK();

  return (s - buf);
}

ssize_t cpufreq_perflock_show_perf_unlock(struct cpufreq_policy* policy, char* buf)
{
  char* s = buf;
  char* end = buf + PAGE_SIZE;
  struct rb_node* n;
  
  MUTEX_LOCK();
  
  for (n = rb_first(&user_perf_locks); n != NULL; n = rb_next(n))
  {
    struct user_perf_lock* l = rb_entry(n, struct user_perf_lock, node);
    if (!perf_lock_active(&l->perf_lock))
    {
      s += snprintf(s, end - s, "%s ", l->name);
    }
  }
  s += snprintf(s, end - s, "\n");
  
  MUTEX_UNLOCK();

  return (s - buf);
}

ssize_t cpufreq_perflock_store_perf_lock(struct cpufreq_policy* policy, const char* buf, size_t count)
{
  struct user_perf_lock* l = NULL;
  int r;

  MUTEX_LOCK();
  
  l = lookup_perf_lock_name(buf, 1);
  if (IS_ERR(l))
  {
    count = PTR_ERR(l);
    goto exit;
  }
  
  if (perf_lock_active(&l->perf_lock))
  {
    if (debug_mask & DEBUG_ACCESS)
      pr_info("perf_lock_store: %s is already locked.\n", l->name);
    goto exit;
  }
  
  l->perf_lock.flags = PERF_LOCK_LOCKED;
  user_perf_lock_count += 1;
  
  r = cpufreq_perflock_set_governor(policy, "performance");
  if (0 > r)
  {
    if (debug_mask & DEBUG_ERROR)
      pr_err("perf_lock_store: failed to set governor as 'performance' (%d)\n", r);
  }
  
  if (debug_mask & DEBUG_ACCESS)
    pr_info("perf_lock_store: %s, 0x%08X, lock count is %d\n", l->name, l->perf_lock.flags, user_perf_lock_count);
    
exit:
  MUTEX_UNLOCK();
  
  return count;
}

ssize_t cpufreq_perflock_store_perf_unlock(struct cpufreq_policy* policy, const char* buf, size_t count)
{
  struct user_perf_lock* l = NULL;

  MUTEX_LOCK();
  
  l = lookup_perf_lock_name(buf, 0);
  if (IS_ERR(l))
  {
    count = PTR_ERR(l);
    goto exit;
  }
  
  if (!perf_lock_active(&l->perf_lock))
  {
    if (debug_mask & DEBUG_ACCESS)
      pr_info("perf_unlock_store: %s is already unlocked.\n", l->name);
    goto exit;
  }
  
  l->perf_lock.flags = PERF_LOCK_UNLOCKED;
  user_perf_lock_count -= 1;
  
  if (0 == user_perf_lock_count)
  {
    int r = cpufreq_perflock_set_governor(policy, "ondemand");
    if (0 > r)
    {
      if (debug_mask & DEBUG_ERROR)
        pr_err("perf_unlock_store: failed to set governor as 'ondemand' (%d)\n", r);
    }
    cpufreq_ondemand_set_sampling_rate(ondemand_params.sampling_rate);
  }
  
  if (debug_mask & DEBUG_ACCESS)
    pr_info("perf_unlock_store: %s, 0x%08X, lock count is %d\n", l->name, l->perf_lock.flags, user_perf_lock_count);

exit:  
  MUTEX_UNLOCK();
  
  return count;
}

#endif /* #ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE */

static int __init cpufreq_perflock_userspace_init(void)
{
  user_perf_lock_count = 0;
  printk(KERN_ERR "cpufreq_perflock_userspace_init\n");
  return 0;
}

static void __exit cpufreq_perflock_userspace_exit(void)
{
  printk(KERN_ERR "cpufreq_perflock_userspace_exit\n");
}

MODULE_AUTHOR ("Jeon Jae Woo <jean1225@pantech.com>")
MODULE_DESCRIPTION ("CPUfreq performance locks 'userspace'")
MODULE_LICENSE ("GPL")

#ifdef CONFIG_CPU_FREQ_PERFLOCK_USERSPACE
fs_initcall(cpufreq_perflock_userspace_init);
#else
module_init(cpufreq_perflock_userspace_init);
#endif
module_exit(cpufreq_perflock_userspace_exit);
