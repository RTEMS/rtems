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
  ISR_Level      level;
  rtems_interval tmp;

  switch ( option ) {
    case RTEMS_CLOCK_GET_TOD:
      if ( !_TOD_Is_set )
        return RTEMS_NOT_DEFINED;

      *(rtems_time_of_day *)time_buffer = _TOD_Current;
      return RTEMS_SUCCESSFUL;

    case RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH:
      if ( !_TOD_Is_set )
        return RTEMS_NOT_DEFINED;

      *(rtems_interval *)time_buffer = _TOD_Seconds_since_epoch;
      return RTEMS_SUCCESSFUL;

    case RTEMS_CLOCK_GET_TICKS_SINCE_BOOT:
      *(rtems_interval *)time_buffer = _Watchdog_Ticks_since_boot;
      return RTEMS_SUCCESSFUL;

    case RTEMS_CLOCK_GET_TICKS_PER_SECOND:
      *(rtems_interval *)time_buffer = _TOD_Ticks_per_second;
      return RTEMS_SUCCESSFUL;

    case RTEMS_CLOCK_GET_TIME_VALUE:
      if ( !_TOD_Is_set )
        return RTEMS_NOT_DEFINED;

      _ISR_Disable( level );
        ((rtems_clock_time_value *)time_buffer)->seconds =
          _TOD_Seconds_since_epoch;
        tmp = _TOD_Current.ticks;
      _ISR_Enable( level );

      tmp *= _TOD_Microseconds_per_tick;
      ((rtems_clock_time_value *)time_buffer)->microseconds = tmp;

      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* should never get here */

}
