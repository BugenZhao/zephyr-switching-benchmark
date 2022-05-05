# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(myapp)

target_sources(app PRIVATE src/main.cc src/xip.cc src/preempt.cc src/fib.cc src/user_blinky.cc src/many_tasks.cc src/my_mutex.cc src/mutex.cc)
