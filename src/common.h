#ifndef MY_COMMON_H
#define MY_COMMON_H

#include <stdio.h>
#include <zephyr.h>

#define print_current()           \
  printf("%s, in %s\n", __func__, \
         k_is_user_context() ? "user mode" : "kernel mode");

#endif
