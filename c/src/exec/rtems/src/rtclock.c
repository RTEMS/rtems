/*
 *  Clock Manager
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/clock.h>
#include <rtems/config.h>
#include <rtems/isr.h>
#include <rtems/thread.h>
#include <rtems/tod.h>
#include <rtems/watchdog.h>

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
      if ( !_TOD_Is_set() )
        return( RTEMS_NOT_DEFINED );

      *(rtems_time_of_day *)time_buffer = _TOD_Current;
      return( RTEMS_SUCCESSFUL );

    case RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH:
      if ( !_TOD_Is_set() )
        return( RTEMS_NOT_DEFINED );

      *(rtems_interval *)time_buffer = _TOD_Seconds_since_epoch;
      return( RTEMS_SUCCESSFUL );

    case RTEMS_CLOCK_GET_TICKS_SINCE_BOOT:
      *(rtems_interval *)time_buffer = _TOD_Ticks_since_boot;
      return( RTEMS_SUCCESSFUL );

    case RTEMS_CLOCK_GET_TICKS_PER_SECOND:
      *(rtems_interval *)time_buffer = _TOD_Ticks_per_second;
      return( RTEMS_SUCCESSFUL );

    case RTEMS_CLOCK_GET_TIME_VALUE:
      if ( !_TOD_Is_set() )
        return( RTEMS_NOT_DEFINED );

      _ISR_Disable( level );
        ((rtems_clock_time_value *)time_buffer)->seconds =
          _TOD_Seconds_since_epoch;
        tmp = _TOD_Current.ticks;
      _ISR_Enable( level );

      tmp *= _Configuration_Table->microseconds_per_tick;
      ((rtems_clock_time_value *)time_buffer)->microseconds = tmp;

      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_SUCCESSFUL );   /* should never get here */

}

/*PAGE
 *
 *  rtems_clock_set
 *
 *  This directive sets the date and time for this node.
 *
 *  Input parameters:
 *    time_buffer - pointer to the time and date structure
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_clock_set(
  rtems_time_of_day *time_buffer
)
{
  rtems_status_code      local_result;
  rtems_interval seconds;

  local_result = _TOD_Validate( time_buffer );
  if ( rtems_is_status_successful( local_result ) ) {
    seconds = _TOD_To_seconds( time_buffer );
    _Thread_Disable_dispatch();
      _TOD_Set( time_buffer, seconds );
    _Thread_Enable_dispatch();

  }
  return( local_result );
}

/*PAGE
 *
 *  rtems_clock_tick
 *
 *  This directive notifies the executve that a tick has occurred.
 *  When the tick occurs the time manager updates and maintains
 *  the calendar time, timeslicing, and any timeout delays.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always succeeds
 *
 *  NOTE: This routine only works for leap-years through 2099.
 */

rtems_status_code rtems_clock_tick( void )
{
  _TOD_Tickle_ticks();

  _Watchdog_Tickle_ticks();

  _Thread_Tickle_timeslice();

  if ( _Thread_Is_context_switch_necessary() &&
       _Thread_Is_dispatching_enabled() )
    _Thread_Dispatch();

  return( RTEMS_SUCCESSFUL );
}
