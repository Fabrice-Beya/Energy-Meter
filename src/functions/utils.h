#ifndef UTILS
#define UTILS

#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "enums.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

/**
 * Function used to reset the watchdog timer.
 */
void reset_wdt()
{
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
}

#endif