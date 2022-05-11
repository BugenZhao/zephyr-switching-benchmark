#include <drivers/gpio.h>
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>
#include "common.h"
#include "fib.h"
#include "many_tasks.h"
#include "mutex.h"
#include "preempt.h"
#include "user_blinky.h"
#include "xip.h"
#include "user_test.h"

#define THREAD_STACKSIZE 2048

struct k_thread thread;
K_THREAD_STACK_DEFINE(thread_stack, THREAD_STACKSIZE);

struct k_thread thread_2;
K_THREAD_STACK_DEFINE(thread_2_stack, THREAD_STACKSIZE);

void bench(bool user, k_thread_entry_t func, int times) {
  do_bench(user, &thread, thread_stack, THREAD_STACKSIZE, func, -1, times);
}
void main_fib_bench() {
  const int BENCH_TIMES = 3;

  k_object_access_all_grant(&fib_k_sem);

  for (int i = 0; i < 2; i++) {
    bool user = i % 2 == 1;

    bench(user, thread_function_fib_only, BENCH_TIMES);
    // if (!user) {
    //   bench(user, thread_function_fib_with_thread_get_xip_clear,
    //   BENCH_TIMES);
    // }

    bench(user, thread_function_fib_with_k_sem, BENCH_TIMES);
    // bench(user, thread_function_k_sem_only, BENCH_TIMES);
    bench(user, thread_function_fib_with_sys_sem, BENCH_TIMES);

    // bench(user, thread_function_fib_with_thread_get, BENCH_TIMES);
    // bench(user, thread_function_thread_get_only, BENCH_TIMES);

    k_msleep(1000);
  }
}

void main_preempt() {
  k_thread_create(&thread, thread_stack, THREAD_STACKSIZE,
                  thread_function_blinky, NULL, NULL, NULL, -1, K_INHERIT_PERMS,
                  K_NO_WAIT);

  k_thread_create(&thread_2, thread_2_stack, THREAD_STACKSIZE,
                  thread_function_busy, NULL, NULL, NULL, 1, K_INHERIT_PERMS,
                  K_MSEC(2000));
}

#ifdef CONFIG_BOARD_RPI_PICO

K_SEM_DEFINE(blinky_sem, 0, 1);
void main_user_blinky() {
  static const struct gpio_dt_spec led =
      GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

  k_thread_access_grant(&thread, &blinky_sem, led.port);
  k_thread_create(&thread, thread_stack, THREAD_STACKSIZE,
                  thread_function_user_blinky_on_sem, &blinky_sem, (void*)&led,
                  NULL, -1, K_USER, K_NO_WAIT);

  k_thread_access_grant(&thread_2, &blinky_sem);
  k_thread_create(&thread_2, thread_2_stack, THREAD_STACKSIZE,
                  thread_function_user_give_sem, &blinky_sem, NULL, NULL, 1,
                  K_USER, K_MSEC(2000));
}

#endif

void main(void) {
  printf("Welcome, %s!\n\n", CONFIG_BOARD);

  // main_fib_bench();
  // main_preempt();
  // main_user_blinky();
  // many_tasks();
  mutex_bench();
  // mutex_test();
  // user_test();
}
