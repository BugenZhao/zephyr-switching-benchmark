#include "xip.h"
#include <stdio.h>

void* const XIP_CTRL_BASE = (void*)0x14000000;

volatile uint32_t* const XIP_CTRL = XIP_CTRL_BASE;
volatile uint32_t* const XIP_CTR_HIT = XIP_CTRL_BASE + 0x0c;
volatile uint32_t* const XIP_CTR_ACC = XIP_CTRL_BASE + 0x10;

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

void stat_xip_hit() {
  uint32_t hit = *XIP_CTR_HIT;
  uint32_t acc = *XIP_CTR_ACC;

  printf("xip cache stat: acc %10u, hit %10u\n", acc, hit);
}
