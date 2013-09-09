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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

rtems_interval rtems_clock_get_ticks_per_second(void)
{
  return TOD_MICROSECONDS_PER_SECOND /
    rtems_configuration_get_microseconds_per_tick();
}
