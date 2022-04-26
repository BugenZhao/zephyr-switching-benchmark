#include <drivers/gpio.h>
#include <stdio.h>
#include <zephyr.h>
#include <zephyr/sys/sem.h>

#define USER_STACKSIZE 2048

#define SLEEP_TIME_MS 250

#if defined(CONFIG_BOARD_RPI_PICO)

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void blinky(void) {
  int ret;

  if (!device_is_ready(led.port)) {
    return;
  }

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return;
  }

  int count = 0;

  while (1) {
    count += 1;
    ret = gpio_pin_toggle_dt(&led);
    printf("Blink %d\n", count);
    if (ret < 0) {
      return;
    }
    k_msleep(SLEEP_TIME_MS);
  }
}
#else

void blinky(void) {
  while (1) {
    printf("Blink!\n");
    k_msleep(SLEEP_TIME_MS);
  }
}

#endif

struct k_thread thread;
K_THREAD_STACK_DEFINE(user_stack, USER_STACKSIZE);

static void user_function(void* p1, void* p2, void* p3) {
  printf("Hello World myapp from %s (%s)\n",
         k_is_user_context() ? "UserSpace!" : "privileged mode.", CONFIG_BOARD);

  if (k_is_user_context()) {
    k_msleep(1500);
  } else {
    return;
  }

  // __ASSERT(k_is_user_context(), "User mode execution was expected");
}

struct k_thread user_thread_3000;
K_THREAD_STACK_DEFINE(user_stack_3000, USER_STACKSIZE);

static void user_function_3000(void* p1, void* p2, void* p3) {
  printf("Hello World myapp 3000 from %s (%s)\n",
         k_is_user_context() ? "UserSpace!" : "privileged mode.", CONFIG_BOARD);

  if (k_is_user_context()) {
    k_msleep(1000);
  } else {
    return;
  }

  // __ASSERT(k_is_user_context(), "User mode execution was expected");
}

void main(void) {
  k_thread_create(&thread, user_stack, USER_STACKSIZE, user_function, NULL,
                  NULL, NULL, -1, K_INHERIT_PERMS | K_USER, K_NO_WAIT);
  k_thread_create(&user_thread_3000, user_stack_3000, USER_STACKSIZE,
                  user_function_3000, NULL, NULL, NULL, -1,
                  K_INHERIT_PERMS | K_USER, K_NO_WAIT);
  k_thread_join(&thread, K_FOREVER);
  k_thread_join(&user_thread_3000, K_FOREVER);

  blinky();
}
