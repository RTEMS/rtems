/**
 *  @file
 *
 *  @brief Announce a Clock Tick
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/timecounter.h>

rtems_status_code rtems_clock_tick( void )
{
  _Timecounter_Tick_simple(
    rtems_configuration_get_microseconds_per_tick(),
    0
  );

  return RTEMS_SUCCESSFUL;
}
