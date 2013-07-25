/**
 *  @file
 *
 *  @brief Set the Current TOD
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/threaddispatch.h>
#include <rtems/score/tod.h>
#include <rtems/config.h>

rtems_status_code rtems_clock_set(
  const rtems_time_of_day *tod
)
{
  if ( !tod )
    return RTEMS_INVALID_ADDRESS;

  if ( _TOD_Validate( tod ) ) {
    Timestamp_Control tod_as_timestamp;
    uint32_t seconds = _TOD_To_seconds( tod );
    uint32_t nanoseconds = tod->ticks
      * rtems_configuration_get_nanoseconds_per_tick();

    _Timestamp_Set( &tod_as_timestamp, seconds, nanoseconds );

    _Thread_Disable_dispatch();
      _TOD_Set_with_timestamp( &tod_as_timestamp );
    _Thread_Enable_dispatch();

    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INVALID_CLOCK;
}
