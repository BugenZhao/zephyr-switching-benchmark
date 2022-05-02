#include "xip.h"
#include <stdio.h>

static auto XIP_CTRL = (volatile uint32_t* const)(XIP_CTRL_BASE);

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
