#include "user_test.h"
#include <stdio.h>
#include <zephyr.h>

struct k_thread user_test_thread_00;
K_THREAD_STACK_DEFINE(user_test_stack_00, 1024);

K_MUTEX_DEFINE(user_test_mutex);
void task_thread_user_test_1(void* p1, void* p2, void* p3) {
  printf("in user mode: %s\n", k_is_user_context() ? "true" : "false");
  k_mutex_lock(&user_test_mutex, K_FOREVER);
}

volatile uint32_t global_count = 0;
void task_thread_user_test_2(void* p1, void* p2, void* p3) {
  printf("in user mode: %s\n", k_is_user_context() ? "true" : "false");
  global_count += 1;
}

void user_test() {
  // k_thread_create(&user_test_thread_00, user_test_stack_00, 1024,
  //                 task_thread_user_test_1, NULL, NULL, NULL, -1, K_USER,
  //                 K_NO_WAIT);
  k_thread_create(&user_test_thread_00, user_test_stack_00, 1024,
                  task_thread_user_test_2, NULL, NULL, NULL, -1, K_USER,
                  K_NO_WAIT);
}
