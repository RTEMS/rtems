/*
 *  Clock Manager - rtems_clock_get_tod
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

rtems_status_code rtems_clock_get_tod(
  rtems_time_of_day  *time_buffer
)
{
  rtems_time_of_day *tmbuf = time_buffer;
  struct tm time;
  struct timeval now;

  if ( !time_buffer )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD.is_set )
    return RTEMS_NOT_DEFINED;

  /* Obtain the current time */
  _TOD_Get_timeval( &now );

  /* Split it into a closer format */
  gmtime_r( &now.tv_sec, &time );

  /* Now adjust it to the RTEMS format */
  tmbuf->year   = time.tm_year + 1900;
  tmbuf->month  = time.tm_mon + 1;
  tmbuf->day    = time.tm_mday;
  tmbuf->hour   = time.tm_hour;
  tmbuf->minute = time.tm_min;
  tmbuf->second = time.tm_sec;
  tmbuf->ticks  = now.tv_usec /
    rtems_configuration_get_microseconds_per_tick();

  return RTEMS_SUCCESSFUL;
}
