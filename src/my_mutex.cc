#include "my_mutex.h"
#include <stdio.h>

// #define MY_MUTEX_LOG

#ifdef MY_MUTEX_LOG
#define mutex_log(fmt, ...) printf("" fmt, ##__VA_ARGS__)
#else
#define mutex_log(fmt, ...)
#endif

int my_mutex_lock(struct my_mutex* mutex, k_tid_t tid, k_timeout_t timeout) {
  auto tid_val = (atomic_val_t)tid;
  int ret = 0;

  // Fast path for uncontended mutex.
  if (likely(atomic_cas(&mutex->futex.val, MY_MUTEX_UNCONTENDED, tid_val))) {
    mutex_log("lock(%p): fast path, uncountended, done\n", tid);
    return 0;
  }

  while (true) {
    atomic_val_t old_val = atomic_get(&mutex->futex.val);
    if (old_val == MY_MUTEX_UNCONTENDED) {
      if (!likely(atomic_cas(&mutex->futex.val, old_val, tid_val))) {
        mutex_log("lock(%p): cas retry\n", tid);
        continue;
      }
      mutex_log("lock(%p): uncountended\n", tid);
    } else {
      if ((old_val & 0x1) == 0) {  // no waiting
        if (!likely(atomic_cas(&mutex->futex.val, old_val, old_val | 0x1))) {
          mutex_log("lock(%p): cas retry\n", tid);
          continue;
        }
        mutex_log("lock(%p): countended, mark waiting bit\n", tid);
      } else {
        mutex_log("lock(%p): countended, waiting bit already marked\n", tid);
      }
      mutex_log("lock(%p): start to wait\n", tid);
      ret = k_futex_wait(&mutex->futex, old_val | 0x1, timeout);
      if (likely(ret == 0 || ret == -EAGAIN)) {
        mutex_log("lock(%p): wait ok, retry locking\n", tid);
        continue;
      }
    }

    break;
  }

  mutex_log("lock(%p): done\n", tid);

  return ret;
}

int my_mutex_unlock(struct my_mutex* mutex, k_tid_t tid) {
  auto tid_val = (atomic_val_t)tid;
  int ret = 0;

  // Fast path for no waiting threads.
  if (likely(atomic_cas(&mutex->futex.val, tid_val, MY_MUTEX_UNCONTENDED))) {
    mutex_log("unlock(%p): fast path, no waiting, done\n", tid);
    return 0;
  }

  while (true) {
    atomic_val_t old_val = atomic_get(&mutex->futex.val);
    if (likely(old_val == (tid_val | 0x1))) {
      if (!likely(
              atomic_cas(&mutex->futex.val, old_val, MY_MUTEX_UNCONTENDED))) {
        mutex_log("unlock(%p): cas retry\n", tid);
        continue;
      }
      ret = k_futex_wake(&mutex->futex, true);
      mutex_log("unlock(%p): has waiting, waked\n", tid);
    } else if (old_val == tid_val) {
      if (!likely(
              atomic_cas(&mutex->futex.val, old_val, MY_MUTEX_UNCONTENDED))) {
        mutex_log("unlock(%p): cas retry\n", tid);
        continue;
      }
      mutex_log("unlock(%p): no waiting\n", tid);
    } else if (old_val == MY_MUTEX_UNCONTENDED) {
      ret = -EINVAL;
    } else {
      ret = -EPERM;
    }

    break;
  }

  mutex_log("unlock(%p): done\n", tid);

  if (likely(ret >= 0)) {
    return 0;
  }
  return ret;
}
