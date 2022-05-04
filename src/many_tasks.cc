#include "many_tasks.h"
#include <stdio.h>
#include <zephyr.h>
#include <zephyr/app_memory/app_memdomain.h>
#include "common.h"
#include "fib.h"

// #define MANY_TASKS_YIELD_COUNT

#ifdef MANY_TASKS_YIELD_COUNT
K_APPMEM_PARTITION_DEFINE(my_partition);
K_APP_BMEM(my_partition) uint32_t yield_count = 0;
#endif

void thread_function_always_yield(void* p1, void* p2, void* p3) {
  while (true) {
#ifdef MANY_TASKS_YIELD_COUNT
    yield_count += 1;
#endif
    k_yield();
  }
}

void thread_function_busy_work(void* p1, void* p2, void* p3) {
#ifdef MANY_TASKS_YIELD_COUNT
  yield_count = 0;
#endif
  thread_function_fib_only(p1, p2, p3);
#ifdef MANY_TASKS_YIELD_COUNT
  printf("yield count = %u\n", yield_count);
#endif
}

#include "spawn.cc.gen"

void many_tasks() {
#ifdef MANY_TASKS_YIELD_COUNT
  k_mem_domain domain;
  k_mem_partition* parts[] = {&my_partition};
  k_mem_domain_init(&domain, ARRAY_SIZE(parts), parts);

  k_mem_domain_add_thread(&domain, k_current_get());
#endif

  k_sched_time_slice_set(10, 1);
  printf("test with many tasks\n");

  auto yield_user = false;
  auto busy_user = false;

  spawn_many_tasks(yield_user);

  do_bench(busy_user, &thread_00, stack_00, sizeof(stack_00),
           thread_function_busy_work, 1, 5);
}
