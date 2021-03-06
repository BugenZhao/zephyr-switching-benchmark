#include "preempt.h"
#include <drivers/gpio.h>
#include <stdio.h>
#include <zephyr.h>

void thread_function_blinky(void* p1, void* p2, void* p3) {
#ifdef CONFIG_BOARD_RPI_PICO

  static const struct gpio_dt_spec led =
      GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

  int ret = 0;

  ret = device_is_ready(led.port);
  __ASSERT(ret, "led is not ready");

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  __ASSERT(ret == 0, "failed to configure led");

  while (true) {
    gpio_pin_toggle_dt(&led);
    printf("led toggle\n");
    k_msleep(250);
  }

#endif
}

void thread_function_busy(void* p1, void* p2, void* p3) {
  volatile uint32_t a = 0;
  while (true) {
    if (a % (1 << 22) == 0) {
      printf("busy %x\n", a);
    }
    a += 1;
  }
}
