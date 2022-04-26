#include <drivers/gpio.h>
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>

#define THREAD_STACKSIZE 2048

struct k_thread thread;
K_THREAD_STACK_DEFINE(thread_stack, THREAD_STACKSIZE);

#define print_current()           \
  printf("%s, in %s\n", __func__, \
         k_is_user_context() ? "user mode" : "kernel mode");

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

const int fib_len = 128;
const int fib_loop = 20000000;
const int syscall_step = 1024;
const int sem_limit = fib_loop / syscall_step + 10;

void thread_function_sem_with_fib(void* p1, void* p2, void* p3) {
  print_current();

  SYS_SEM_DEFINE(sem, 0, sem_limit);

  const int len = fib_len;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < fib_loop; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];

    if (i % syscall_step == 0) {
      sys_sem_give(&sem);
    }
  }

  printf("done, fib[i] = %u, sem value = %u\n", fib[i % len],
         sys_sem_count_get(&sem));
}

void thread_function_fib_only(void* p1, void* p2, void* p3) {
  print_current();

  const int len = fib_len;
  uint32_t fib[len];
  fib[0] = 0;
  fib[1] = 1;

  register int i = 2;
  for (; i < fib_loop; ++i) {
    fib[i % len] = fib[(i + len - 1) % len] + fib[(i + len - 2) % len];
  }

  printf("done, fib[i] = %u\n", fib[i % len]);
}

void thread_function_sem_only(void* p1, void* p2, void* p3) {
  print_current();

  SYS_SEM_DEFINE(sem, 0, sem_limit);

  register int i = 2;
  for (; i < fib_loop; ++i) {
    if (i % syscall_step == 0) {
      sys_sem_give(&sem);
    }
  }

  printf("done, sem value = %u\n", sys_sem_count_get(&sem));
}

void stat(bool user, void* func, int times) {
  while (times--) {
    uint32_t user_option = user ? K_USER : 0;

    int32_t cycle_start = sys_clock_cycle_get_32();

    k_thread_create(&thread, thread_stack, THREAD_STACKSIZE, func, NULL, NULL,
                    NULL, -1, K_INHERIT_PERMS | user_option, K_NO_WAIT);
    k_thread_join(&thread, K_FOREVER);

    uint32_t cycle_end = sys_clock_cycle_get_32();

    printf("start %010u, end %010u => %10u\n\n", cycle_start, cycle_end,
           cycle_end - cycle_start);
  }
}

void main(void) {
  printf("Welcome, %s!\n\n", CONFIG_BOARD);

  for (int i = 0;; i++) {
    bool user = i % 2 == 0;
    stat(user, thread_function_sem_with_fib, 5);
    stat(user, thread_function_fib_only, 5);
    stat(user, thread_function_sem_only, 5);

    k_msleep(1000);
  }
}
