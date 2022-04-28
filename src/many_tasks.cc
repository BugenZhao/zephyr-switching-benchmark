#include "many_tasks.h"
#include <stdio.h>
#include <zephyr.h>
#include "common.h"
#include "fib.h"

void thread_function_always_yield(void* p1, void* p2, void* p3) {
  while (true) {
    k_yield();
  }
}

void thread_function_busy_work(void* p1, void* p2, void* p3) {
  thread_function_fib_only(p1, p2, p3);
}

#include "spawn.cc.gen"

void many_tasks() {
  k_sched_time_slice_set(10, 1);
  printf("test with many tasks\n");

  spawn_many_tasks();

  do_bench(true, &thread_00, stack_00, sizeof(stack_00),
           thread_function_busy_work, 1, 5);
}
