#ifndef MY_MY_MUTEX_H
#define MY_MY_MUTEX_H

#include <sys/atomic.h>
#include <zephyr.h>
#include <zephyr/types.h>

#define MY_MUTEX_UNCONTENDED 0

struct my_mutex {
  struct k_futex futex;
};

#define MY_MUTEX_DEFINE(_name) \
  struct my_mutex _name = {.futex = {MY_MUTEX_UNCONTENDED}}

int my_mutex_lock(struct my_mutex* mutex, k_tid_t tid, k_timeout_t timeout);

int my_mutex_unlock(struct my_mutex* mutex, k_tid_t tid);

#endif
