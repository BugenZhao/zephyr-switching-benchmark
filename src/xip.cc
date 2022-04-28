#include "xip.h"
#include <stdio.h>

const auto XIP_CTRL_BASE = (uint8_t*)0x14000000;

auto XIP_CTRL = (volatile uint32_t* const)(XIP_CTRL_BASE);
auto XIP_CTR_FLUSH = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x04);
auto XIP_CTR_HIT = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x0c);
auto XIP_CTR_ACC = (volatile uint32_t* const)(XIP_CTRL_BASE + 0x10);

void config_xip(bool enable) {
  printf("current xip ctrl: %x\n", *XIP_CTRL);
  if (enable) {
    *XIP_CTRL |= 0x1;
  } else {
    *XIP_CTRL &= ~0x1;
  }
  printf("after xip ctrl: %x\n", *XIP_CTRL);
  printf("xip cache %s!\n", enable ? "enabled" : "disabled");
}

void clear_xip_cache() {
  *XIP_CTR_FLUSH = 0x1;
  *XIP_CTR_FLUSH;
}

void clear_xip_stat() {
  *XIP_CTR_ACC = 0;
  *XIP_CTR_HIT = 0;
}

void stat_xip_hit() {
  auto hit = *XIP_CTR_HIT;
  auto acc = *XIP_CTR_ACC;

  printf("xip cache stat: acc %10u, hit %10u\n", acc, hit);
}
