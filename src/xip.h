#ifndef MY_XIP_H
#define MY_XIP_H

#include <zephyr.h>
#include <stdio.h>

static const auto XIP_CTRL_BASE = (uint8_t*)0x14000000;

static auto XIP_CTR_FLUSH = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x04);
static auto XIP_CTR_HIT = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x0c);
static auto XIP_CTR_ACC = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x10);

inline void clear_xip_cache() {
  *XIP_CTR_FLUSH = 0x1;
  *XIP_CTR_FLUSH;
}

inline void clear_xip_stat() {
  *XIP_CTR_ACC = 0;
  *XIP_CTR_HIT = 0;
}

inline void stat_xip_hit() {
  auto hit = *XIP_CTR_HIT;
  auto acc = *XIP_CTR_ACC;

  printf("xip cache stat: acc %10u, hit %10u\n", acc, hit);
}

void config_xip(bool enable);

#endif
