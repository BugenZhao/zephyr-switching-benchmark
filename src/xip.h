#ifndef MY_XIP_H
#define MY_XIP_H

#include <zephyr.h>

void config_xip(bool enable);
void stat_xip_hit();

#endif
