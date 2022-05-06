#ifndef MY_XIP_H
#define MY_XIP_H

#include <stdio.h>
#include <zephyr.h>

static const auto XIP_CTRL_BASE = (uint8_t*)0x14000000;

static auto XIP_CTR_FLUSH = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x04);
static auto XIP_CTR_HIT = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x0c);
static auto XIP_CTR_ACC = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x10);

inline void clear_xip_cache() {
#ifdef CONFIG_BOARD_RPI_PICO
  *XIP_CTR_FLUSH = 0x1;
  *XIP_CTR_FLUSH;
#endif
}

inline void clear_xip_stat() {
#ifdef CONFIG_BOARD_RPI_PICO
  *XIP_CTR_ACC = 0;
  *XIP_CTR_HIT = 0;
#endif
}

inline void stat_xip_hit() {
#ifdef CONFIG_BOARD_RPI_PICO
  auto hit = *XIP_CTR_HIT;
  auto acc = *XIP_CTR_ACC;

  printf("xip cache stat: acc %10u, hit %10u\n", acc, hit);
#endif
}

void config_xip(bool enable);

#endif
