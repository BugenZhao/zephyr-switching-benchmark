#ifndef MY_COMMON_H
#define MY_COMMON_H

#include <stdio.h>
#include <zephyr.h>
#include "xip.h"

#define print_current()           \
  printf("%s, in %s\n", __func__, \
         k_is_user_context() ? "user mode" : "kernel mode");

#define assert_user() \
  __ASSERT(k_is_user_context(), "%s not in the user context!", __func__);

#endif

inline void do_bench(bool user,
                     k_thread* thread,
                     k_thread_stack_t* stack,
                     size_t stack_size,
                     k_thread_entry_t func,
                     int prio,
                     int times) {
  printf("bench start...\n");
  clear_xip_cache();

  while (times--) {
    auto user_option = user ? K_USER : 0;

    k_thread_create(thread, stack, stack_size, func, NULL, NULL, NULL, prio,
                    K_INHERIT_PERMS | user_option, K_FOREVER);

    clear_xip_stat();

    auto cycle_start = sys_clock_cycle_get_32();

    k_thread_start(thread);
    k_thread_join(thread, K_FOREVER);

    auto cycle_end = sys_clock_cycle_get_32();

    printf("start %10u, end %10u => %10u\n", cycle_start, cycle_end,
           cycle_end - cycle_start);
    stat_xip_hit();
  }
  printf("bench end...\n\n");
}
