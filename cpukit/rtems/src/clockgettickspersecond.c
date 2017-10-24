/**
 *  @file
 *
 *  @brief Obtain Ticks Per Seconds
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>

#undef rtems_clock_get_ticks_per_second

rtems_interval rtems_clock_get_ticks_per_second(void)
{
  return _Watchdog_Ticks_per_second;
}
