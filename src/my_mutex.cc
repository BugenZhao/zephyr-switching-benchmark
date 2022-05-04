#include <sys/atomic.h>
#include <sys_clock.h>
#include <zephyr.h>
#include <zephyr/types.h>

#define MY_MUTEX_UNCONTENDED 0

struct my_mutex {
  struct k_futex futex;
};

#define MY_MUTEX_DEFINE(_name) \
  struct my_mutex _name = {.futex = {MY_MUTEX_UNCONTENDED}}

int my_mutex_lock(struct my_mutex* mutex, k_tid_t tid, k_timeout_t timeout) {
  auto tid_val = (atomic_val_t)tid;
  int ret = 0;

  do {
    atomic_val_t old_val = atomic_get(&mutex->futex.val);
    if (old_val == MY_MUTEX_UNCONTENDED) {
      ret = atomic_cas(&mutex->futex.val, old_val, tid_val);  // 1 if success
    } else {
      ret = k_futex_wait(&mutex->futex, old_val, timeout);
    }
  } while (ret == 0 || ret == -EAGAIN);

  return ret;
}

int my_mutex_unlock(struct my_mutex* mutex, k_tid_t tid) {
  auto tid_val = (atomic_val_t)tid;
  int ret = 0;

  do {
    atomic_val_t old_val = atomic_get(&mutex->futex.val);
    if (old_val == tid_val) {
      ret = atomic_cas(&mutex->futex.val, old_val,
                       MY_MUTEX_UNCONTENDED);  // 1 if success
    } else if (old_val == MY_MUTEX_UNCONTENDED) {
      return -EINVAL;
    } else {
      return -EPERM;
    }
  } while (ret == 0 || ret == -EAGAIN);

  ret = k_futex_wake(&mutex->futex, true);
  if (ret > 0) {
    return 0;
  }
  return ret;
}
