/*  Screen2
 *
 *  This routine generates error screen 2 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include "system.h"

void Screen2()
{
  rtems_time_of_day time;
  rtems_status_code status;

/* errors before clock is set */

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
     "TA1 - rtems_clock_get - RTEMS_NOT_DEFINED -- DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_clock_get before clock is set"
    );
    puts( "TA1 - rtems_clock_get - RTEMS_NOT_DEFINED" );
  }

  status = rtems_task_wake_when( &time );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
"TA1 - rtems_task_wake_when - RTEMS_NOT_DEFINED -- DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_task_wake_when before clock is set"
    );
    puts( "TA1 - rtems_task_wake_when - RTEMS_NOT_DEFINED" );
  }

  status = rtems_timer_fire_when( 0, &time, Delayed_routine, NULL );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
    "TA1 - timer_wake_when - RTEMS_NOT_DEFINED -- DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "task_fire_when before clock is set"
    );
    puts( "TA1 - rtems_timer_fire_when - RTEMS_NOT_DEFINED" );
  }

  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid year"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 15, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid month"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 32, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid day"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 25, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid hour"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 8, 61, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid minute"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 8, 30, 61, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid second"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 8, 30, 45, TICKS_PER_SECOND + 1 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_clock_set with invalid ticks per second"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set successful" );
  puts( " - RTEMS_SUCCESSFUL" );

/* rtems_task_wake_when */

  build_time( &time, 2, 5, 1988, 8, 30, 48, TICKS_PER_SECOND + 1 );
  time.second += 3;
  puts( "TA1 - rtems_task_wake_when - TICKINVALID - sleep about 3 seconds" );

  status = rtems_task_wake_when( &time );
  directive_failed(
    status,
    "rtems_task_wake_when with invalid ticks per second"
  );
  puts( "TA1 - rtems_task_wake_when - TICKINVALID - woke up RTEMS_SUCCESSFUL" );

  build_time( &time, 2, 5, 1961, 8, 30, 48, 0 );
  print_time( "TA1 - rtems_task_wake_when - ", &time, "" );
  status = rtems_task_wake_when( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_task_wake_when with invalid year"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  build_time( &time, 2, 5, 1988, 25, 30, 48, 0 );
  print_time( "TA1 - rtems_task_wake_when - ", &time, "" );
  status = rtems_task_wake_when( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_task_wake_when with invalid hour"
  );
  puts( " - RTEMS_INVALID_CLOCK" );

  rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  print_time( "TA1 - current time - ", &time, "\n" );

  time.month = 1;
  print_time( "TA1 - rtems_task_wake_when - ", &time, "" );
  status = rtems_task_wake_when( &time );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_task_wake_when before current time"
  );
  puts( " - RTEMS_INVALID_CLOCK" );
}
