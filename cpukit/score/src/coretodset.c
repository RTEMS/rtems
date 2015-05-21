/**
 * @file
 *
 * @brief Set Time of Day Given a Timestamp
 *
 * @ingroup ScoreTOD
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/watchdogimpl.h>

void _TOD_Set_with_timestamp(
  const Timestamp_Control *tod_as_timestamp
)
{
  struct timespec ts;
  uint32_t nanoseconds;
  Watchdog_Interval seconds_next;
  Watchdog_Interval seconds_now;
  Watchdog_Header *header;

  _Timestamp_To_timespec( tod_as_timestamp, &ts );
  nanoseconds = ts.tv_nsec;
  seconds_next = ts.tv_sec;

  _Thread_Disable_dispatch();

  seconds_now = _TOD_Seconds_since_epoch();

  _Timecounter_Set_clock( &ts );

  header = &_Watchdog_Seconds_header;

  if ( seconds_next < seconds_now )
    _Watchdog_Adjust_backward( header, seconds_now - seconds_next );
  else
    _Watchdog_Adjust_forward( header, seconds_next - seconds_now );

  _TOD.seconds_trigger = nanoseconds;
  _TOD.is_set = true;

  _Thread_Enable_dispatch();
}
