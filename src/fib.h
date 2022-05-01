#ifndef MY_FIB_H
#define MY_FIB_H

#include <zephyr.h>

extern struct k_sem fib_k_sem;

void thread_function_fib_with_k_sem(void* p1, void* p2, void* p3);

void thread_function_fib_with_sys_sem(void* p1, void* p2, void* p3);

void thread_function_fib_with_xip_clear(void* p1, void* p2, void* p3);

void thread_function_fib_only(void* p1, void* p2, void* p3);

void thread_function_k_sem_only(void* p1, void* p2, void* p3);

void thread_function_fib_with_thread_get(void* p1, void* p2, void* p3);

void thread_function_thread_get_only(void* p1, void* p2, void* p3);

#endif
