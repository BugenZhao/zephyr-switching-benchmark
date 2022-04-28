#ifndef MY_FIB_H
#define MY_FIB_H

void thread_function_full_sem(void* p1, void* p2, void* p3);

void thread_function_fib_with_sem(void* p1, void* p2, void* p3);

void thread_function_fib_with_xip_clear(void* p1, void* p2, void* p3);

void thread_function_fib_only(void* p1, void* p2, void* p3);

void thread_function_sem_only(void* p1, void* p2, void* p3);

#endif
