#include "fib.h"
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>
#include "common.h"

void thread_function_full_sem(void* p1, void* p2, void* p3) {
  print_current();

  SYS_SEM_DEFINE(sem, 0, 1);

  register int loop = 100000;
  register int ret = 0;
  while (loop--) {
    ret = sys_sem_give(&sem);
    __ASSERT(ret == 0, "failed to give sem");

    ret = sys_sem_take(&sem, K_FOREVER);
    __ASSERT(ret == 0, "failed to take sem");
  }

  printf("done\n");
}

const int FIB_LEN = 128;
const int FIB_LOOP = 200000;
const int SYSCALL_STEP = 1024;
const int SEM_LIMIT = FIB_LOOP / SYSCALL_STEP + 10;

void thread_function_sem_with_fib(void* p1, void* p2, void* p3) {
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

void thread_function_sem_only(void* p1, void* p2, void* p3) {
  print_current();

  SYS_SEM_DEFINE(sem, 0, SEM_LIMIT);

  register int i = 2;
  for (; i < FIB_LOOP; ++i) {
    if (i % SYSCALL_STEP == 0) {
      sys_sem_give(&sem);
    }
  }

  printf("done, sem value = %u\n", sys_sem_count_get(&sem));
}
