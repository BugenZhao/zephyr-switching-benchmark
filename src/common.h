#ifndef MY_COMMON_H
#define MY_COMMON_H

#include <stdio.h>
#include <zephyr.h>

#define print_current()           \
  printf("%s, in %s\n", __func__, \
         k_is_user_context() ? "user mode" : "kernel mode");

#define assert_user() \
  __ASSERT(k_is_user_context(), "%s not in the user context!", __func__);

#endif
