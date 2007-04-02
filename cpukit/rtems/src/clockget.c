/*
 *  Clock Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  rtems_clock_get
 *
 *  This directive returns the current date and time.  If the time has
 *  not been set by a tm_set then an error is returned.
 *
 *  Input parameters:
 *    option      - which value to return
 *    time_buffer - pointer to output buffer (a time and date structure
 *                  or an interval)
 *
 *  Output parameters:
 *    time_buffer      - output filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_clock_get(
  rtems_clock_get_options  option,
  void                    *time_buffer
)
{
  if ( !time_buffer )
    return RTEMS_INVALID_ADDRESS;

  switch ( option ) {
    case RTEMS_CLOCK_GET_TOD: {
      struct tm time;
      struct timeval now;
      rtems_time_of_day *tmbuf = (rtems_time_of_day *)time_buffer;

      if ( !_TOD_Is_set )
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
      tmbuf->ticks  = now.tv_usec / _TOD_Microseconds_per_tick;
     
      return RTEMS_SUCCESSFUL;
    }
    case RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH: {
      rtems_interval *interval = (rtems_interval *)time_buffer;

      if ( !_TOD_Is_set )
        return RTEMS_NOT_DEFINED;

      *interval = _TOD_Seconds_since_epoch;
      return RTEMS_SUCCESSFUL;
    }

    case RTEMS_CLOCK_GET_TICKS_SINCE_BOOT: {
      rtems_interval *interval = (rtems_interval *)time_buffer;

      *interval = _Watchdog_Ticks_since_boot;
      return RTEMS_SUCCESSFUL;
    }

    case RTEMS_CLOCK_GET_TICKS_PER_SECOND: {
      rtems_interval *interval = (rtems_interval *)time_buffer;

      *interval = TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick;
      return RTEMS_SUCCESSFUL;
    }

    case RTEMS_CLOCK_GET_TIME_VALUE: {
      struct timeval *time = (struct timeval *)time_buffer;

      if ( !_TOD_Is_set )
        return RTEMS_NOT_DEFINED;

      _TOD_Get_timeval( time );

      return RTEMS_SUCCESSFUL;
    }
  }

  return RTEMS_INTERNAL_ERROR;   /* should never get here */

}
