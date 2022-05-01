#include "fib.h"
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>
#include "common.h"
#include "xip.h"

const int FIB_LEN = 128;
const int FIB_LOOP = 2000000;
const int SYSCALL_STEP = 1024;
const int SEM_LIMIT = FIB_LOOP / SYSCALL_STEP + 10;

K_SEM_DEFINE(fib_k_sem, 0, SEM_LIMIT);

void thread_function_fib_with_k_sem(void* p1, void* p2, void* p3) {
  print_current();
  k_sem_reset(&fib_k_sem);

  const int len = FIB_LEN;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];

    if (i % SYSCALL_STEP == 0) {
      k_sem_give(&fib_k_sem);
    }
  }

  printf("done, fib[i] = %u, sem value = %u\n", fib[i % len],
         k_sem_count_get(&fib_k_sem));
}

void thread_function_fib_with_sys_sem(void* p1, void* p2, void* p3) {
  print_current();

  SYS_SEM_DEFINE(sem, 0, SEM_LIMIT);

  const int len = FIB_LEN;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];

    if (i % SYSCALL_STEP == 0) {
      sys_sem_give(&sem);
    }
  }

  printf("done, fib[i] = %u, sem value = %u\n", fib[i % len],
         sys_sem_count_get(&sem));
}

void thread_function_fib_with_xip_clear(void* p1, void* p2, void* p3) {
  print_current();

  const int len = FIB_LEN;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
loop:
  for (; i < FIB_LOOP; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];

    if (i % SYSCALL_STEP == 0) {
      clear_xip_cache();
    }
  }
loop_end:

  printf("done, fib[i] = %u\n", fib[i % len]);
}

void thread_function_fib_only(void* p1, void* p2, void* p3) {
  print_current();

  const int len = FIB_LEN;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];
  }

  printf("done, fib[i] = %u\n", fib[i % len]);
}

void thread_function_k_sem_only(void* p1, void* p2, void* p3) {
  print_current();
  k_sem_reset(&fib_k_sem);

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    if (i % SYSCALL_STEP == 0) {
      k_sem_give(&fib_k_sem);
    }
  }

  printf("done, sem value = %u\n", k_sem_count_get(&fib_k_sem));
}

void thread_function_fib_with_thread_get(void* p1, void* p2, void* p3) {
  print_current();

  register k_tid_t tid = NULL;

  const int len = FIB_LEN;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];

    if (i % SYSCALL_STEP == 0) {
      tid = k_current_get();
    }
  }

  printf("done, fib[i] = %u, tid = %p\n", fib[i % len], tid);
}

void thread_function_thread_get_only(void* p1, void* p2, void* p3) {
  print_current();

  register k_tid_t tid = NULL;

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    if (i % SYSCALL_STEP == 0) {
      tid = k_current_get();
    }
  }

  printf("done, tid = %p\n", tid);
}
