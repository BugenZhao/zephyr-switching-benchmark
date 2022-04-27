#include <drivers/gpio.h>
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>
#include "fib.h"
#include "preempt.h"
#include "user_blinky.h"
#include "xip.h"

#define THREAD_STACKSIZE 2048

struct k_thread thread;
K_THREAD_STACK_DEFINE(thread_stack, THREAD_STACKSIZE);

struct k_thread thread_2;
K_THREAD_STACK_DEFINE(thread_2_stack, THREAD_STACKSIZE);

void stat(bool user, k_thread_entry_t func, int times) {
  while (times--) {
    auto user_option = user ? K_USER : 0;

    auto cycle_start = sys_clock_cycle_get_32();

    k_thread_create(&thread, thread_stack, THREAD_STACKSIZE, func, NULL, NULL,
                    NULL, -1, K_INHERIT_PERMS | user_option, K_NO_WAIT);
    k_thread_join(&thread, K_FOREVER);

    auto cycle_end = sys_clock_cycle_get_32();

    printf("start %10u, end %10u => %10u\n\n", cycle_start, cycle_end,
           cycle_end - cycle_start);
  }
}

void main_fib_bench() {
  const int BENCH_TIMES = 1;

  config_xip(false);

  for (int i = 0; i < 2; i++) {
    bool user = i % 2 == 0;
    stat(user, thread_function_sem_with_fib, BENCH_TIMES);
    stat(user, thread_function_fib_only, BENCH_TIMES);
    stat(user, thread_function_sem_only, BENCH_TIMES);

    k_msleep(1000);
    stat_xip_hit();
  }

  config_xip(true);

  for (int i = 0; i < 2; i++) {
    bool user = i % 2 == 0;
    stat(user, thread_function_sem_with_fib, BENCH_TIMES);
    stat(user, thread_function_fib_only, BENCH_TIMES);
    stat(user, thread_function_sem_only, BENCH_TIMES);

    k_msleep(1000);
    stat_xip_hit();
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

K_SEM_DEFINE(sem, 0, 1);
void main_user_blinky() {
  static const struct gpio_dt_spec led =
      GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

  k_thread_access_grant(&thread, &sem, led.port);
  k_thread_create(&thread, thread_stack, THREAD_STACKSIZE,
                  thread_function_user_blinky_on_sem, &sem, (void*)&led, NULL, -1,
                  K_USER, K_NO_WAIT);

  k_thread_access_grant(&thread_2, &sem);
  k_thread_create(&thread_2, thread_2_stack, THREAD_STACKSIZE,
                  thread_function_user_give_sem, &sem, NULL, NULL, 1, K_USER,
                  K_MSEC(2000));
}

void main(void) {
  printf("Welcome, %s!\n\n", CONFIG_BOARD);

  // main_fib_bench();
  // main_preempt();
  main_user_blinky();
}
