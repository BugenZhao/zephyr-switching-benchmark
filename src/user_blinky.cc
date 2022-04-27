#include "user_blinky.h"
#include <drivers/gpio.h>
#include <stdio.h>
#include <sys/sem.h>
#include <zephyr.h>
#include "common.h"

void thread_function_user_blinky_on_sem(void* p1, void* p2, void* p3) {
  assert_user();

  auto sem = (k_sem*)p1;
  auto led = (gpio_dt_spec*)p2;

  int ret = 0;

  ret = device_is_ready(led->port);
  __ASSERT(ret, "led is not ready");
  printf("devide ready\n");

  ret = gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
  __ASSERT(ret == 0, "failed to configure led");
  printf("configured led\n");

  while (true) {
    ret = k_sem_take(sem, K_FOREVER);
    __ASSERT(ret == 0, "failed to take sem");

    gpio_pin_toggle_dt(led);
    __ASSERT(ret == 0, "failed to toggle gpio");
  }
}

void thread_function_user_give_sem(void* p1, void* p2, void* p3) {
  assert_user();

  auto sem = (k_sem*)p1;

  while (true) {
    k_sem_give(sem);
    printf("user %p: give sem!\n", k_current_get());
    k_msleep(1000);
  }
}
