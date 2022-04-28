#ifndef MY_XIP_H
#define MY_XIP_H

#include <zephyr.h>

void config_xip(bool enable);
void clear_xip_cache();
void clear_xip_stat();
void stat_xip_hit();

#endif
