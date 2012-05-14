/*  Screen4
 *
 *  This routine generates error screen 4 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

void Screen4()
{
  rtems_event_set   event_out;
  rtems_time_of_day time;
  struct timeval    tv;
  time_t            seconds;
  rtems_status_code status;

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_event_receive NULL param"
  );
  puts( "TA1 - rtems_event_receive - NULL param - RTEMS_INVALID_ADDRESS" );

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ALL)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( all conditions )" );

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_EVENT_ANY | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ANY)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( any condition )" );

  puts( "TA1 - rtems_event_receive - timeout in 3 seconds" );
  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second(),
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_event_receive"
  );
  puts( "TA1 - rtems_event_receive - woke up with RTEMS_TIMEOUT" );

  status = rtems_event_send( 100, RTEMS_EVENT_16 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_event_send with illegal id"
  );
  puts( "TA1 - rtems_event_send - RTEMS_INVALID_ID" );

  puts( "TA1 - rtems_task_wake_after - sleep 1 second - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  build_time( &time, 2, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  puts( " - RTEMS_SUCCESSFUL" );

  status = rtems_clock_get_tod_timeval( &tv );
  directive_failed( status, "clock_get_tod_timeval OK" );

  seconds = tv.tv_sec;
  printf( "TA1 - current time - %s\n", ctime(&seconds) );

}
