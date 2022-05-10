#include "mutex.h"
#include <stdio.h>
#include <sys/mutex.h>
#include <zephyr.h>
#include <zephyr/app_memory/app_memdomain.h>
#include "common.h"
#include "my_mutex.h"

K_APPMEM_PARTITION_DEFINE(my_partition);
K_APP_BMEM(my_partition) volatile uint32_t counter = 0;
K_APP_BMEM(my_partition) SYS_MUTEX_DEFINE(counter_sys_mutex);
K_APP_BMEM(my_partition) MY_MUTEX_DEFINE(counter_my_mutex);

#define LOCK_TIMES 100000

void thread_function_sys_mutex(void* p1, void* p2, void* p3) {
  print_current();
  counter = 0;

  int ret;
  bool end = false;

  register auto tid = k_current_get();

  while (true) {
    ret = sys_mutex_lock(&counter_sys_mutex, K_FOREVER);
    // __ASSERT(ret == 0, "failed to lock mutex");

    counter += 1;
    end = counter >= LOCK_TIMES;

    ret = sys_mutex_unlock(&counter_sys_mutex);
    // __ASSERT(ret == 0, "failed to unlock mutex");

    if (unlikely(end)) {
      break;
    }
  }

  printf("done. tid = %p, counter = %u\n", tid, counter);
}

void thread_function_my_mutex(void* p1, void* p2, void* p3) {
  print_current();
  counter = 0;

  int ret;
  bool end = false;

  register auto tid = k_current_get();

  while (true) {
    ret = my_mutex_lock(&counter_my_mutex, tid, K_FOREVER);
    // __ASSERT(ret == 0, "failed to lock mutex");

    counter += 1;
    end = counter >= LOCK_TIMES;

    ret = my_mutex_unlock(&counter_my_mutex, tid);
    // __ASSERT(ret == 0, "failed to unlock mutex");

    if (unlikely(end)) {
      break;
    }
  }

  printf("done. tid = %p, counter = %u\n", tid, counter);
}

void thread_function_no_mutex(void* p1, void* p2, void* p3) {
  print_current();
  counter = 0;

  bool end = false;

  register auto tid = k_current_get();

  while (true) {
    counter += 1;
    end = counter >= LOCK_TIMES;

    if (unlikely(end)) {
      break;
    }
  }

  printf("done. tid = %p, counter = %u\n", tid, counter);
}

void thread_function_my_mutex_test(void* p1, void* p2, void* p3) {
  register auto tid = k_current_get();

  int ret;

  auto sleep_ms = (int32_t)(intptr_t)p1;
  if (sleep_ms > 0) {
    k_msleep(sleep_ms);
  }

  ret = my_mutex_lock(&counter_my_mutex, tid, K_FOREVER);
  __ASSERT(ret == 0, "failed to lock mutex");

  k_msleep(200);

  ret = my_mutex_unlock(&counter_my_mutex, tid);
  __ASSERT(ret == 0, "failed to unlock mutex");
}

struct k_thread mutex_thread_00;
K_THREAD_STACK_DEFINE(mutex_stack_00, 1024);

struct k_thread mutex_thread_01;
K_THREAD_STACK_DEFINE(mutex_stack_01, 1024);

struct k_thread mutex_thread_02;
K_THREAD_STACK_DEFINE(mutex_stack_02, 1024);

struct k_thread mutex_thread_03;
K_THREAD_STACK_DEFINE(mutex_stack_03, 1024);

struct k_thread mutex_thread_98;
K_THREAD_STACK_DEFINE(mutex_stack_98, 1024);

struct k_thread mutex_thread_99;
K_THREAD_STACK_DEFINE(mutex_stack_99, 1024);

#define configure_domain()                                \
  {                                                       \
    k_mem_domain domain;                                  \
    k_mem_partition* parts[] = {&my_partition};           \
    k_mem_domain_init(&domain, ARRAY_SIZE(parts), parts); \
    k_mem_domain_add_thread(&domain, k_current_get());    \
  }

void mutex_bench() {
  const auto times = 10;

  configure_domain();

  do_bench(true, &mutex_thread_98, mutex_stack_98, 1024,
           thread_function_no_mutex, -1, times);
  do_bench(false, &mutex_thread_99, mutex_stack_99, 1024,
           thread_function_no_mutex, -1, times);

  do_bench(true, &mutex_thread_00, mutex_stack_00, 1024,
           thread_function_my_mutex, -1, times);
  do_bench(true, &mutex_thread_01, mutex_stack_01, 1024,
           thread_function_sys_mutex, -1, times);
  do_bench(false, &mutex_thread_02, mutex_stack_02, 1024,
           thread_function_my_mutex, -1, times);
  do_bench(false, &mutex_thread_03, mutex_stack_03, 1024,
           thread_function_sys_mutex, -1, times);
}

void mutex_test() {
  configure_domain();

  k_thread_create(&mutex_thread_00, mutex_stack_00, 1024,
                  thread_function_my_mutex_test, (void*)0, NULL, NULL, -1,
                  K_USER | K_INHERIT_PERMS, K_NO_WAIT);

  k_thread_create(&mutex_thread_01, mutex_stack_01, 1024,
                  thread_function_my_mutex_test, (void*)50, NULL, NULL, -1,
                  K_USER | K_INHERIT_PERMS, K_NO_WAIT);

  k_thread_create(&mutex_thread_02, mutex_stack_02, 1024,
                  thread_function_my_mutex_test, (void*)100, NULL, NULL, -1,
                  K_USER | K_INHERIT_PERMS, K_NO_WAIT);

  k_thread_join(&mutex_thread_00, K_FOREVER);
  k_thread_join(&mutex_thread_01, K_FOREVER);
  k_thread_join(&mutex_thread_02, K_FOREVER);

  printf("mutex test done\n");
}
