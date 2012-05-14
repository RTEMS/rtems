/*  Screen2
 *
 *  This routine generates error screen 2 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2011.
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

void Screen2()
{
  rtems_time_of_day time;
  rtems_interval    interval;
  struct timeval    tv;
  rtems_status_code status;

  puts( "TA1 - rtems_clock_get_tod - RTEMS_INVALID_ADDRESS" );
  status = rtems_clock_get_tod( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_get_tod NULL param"
  );

/* errors before clock is set */

  status = rtems_clock_get_tod( &time );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
     "TA1 - rtems_clock_get_tod - RTEMS_NOT_DEFINED -- "
         "DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_clock_get_tod before clock is set #1"
    );
    puts( "TA1 - rtems_clock_get_tod - RTEMS_NOT_DEFINED" );
  }

  puts( "TA1 - rtems_clock_get_seconds_since_epoch - RTEMS_INVALID_ADDRESS" );
  status = rtems_clock_get_seconds_since_epoch( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_get_seconds_since_epoch NULL param"
  );

  status = rtems_clock_get_seconds_since_epoch( &interval );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
     "TA1 - rtems_clock_get_seconds_since_epoch - RTEMS_NOT_DEFINED -- "
         "DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_clock_get_seconds_before_epoch"
    );
    puts( "TA1 - rtems_clock_get_seconds_since_epoch - RTEMS_NOT_DEFINED" );
  }

  puts( "TA1 - rtems_clock_get_uptime - RTEMS_INVALID_ADDRESS" );
  status = rtems_clock_get_uptime( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_get_uptime NULL param"
  );

  puts( "TA1 - rtems_clock_get_tod_timeval - RTEMS_INVALID_ADDRESS" );
  status = rtems_clock_get_tod_timeval( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_get_tod_timeval NULL param"
  );

  status = rtems_clock_get_tod_timeval( &tv );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
     "TA1 - rtems_clock_get_tod_timeval - RTEMS_NOT_DEFINED -- "
         "DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_clock_get_timeval"
    );
    puts( "TA1 - rtems_clock_get_tod_timeval - RTEMS_NOT_DEFINED" );
  }

  puts( "TA1 - rtems_clock_set_nanoseconds_extension - RTEMS_INVALID_ADDRESS" );
  status = rtems_clock_set_nanoseconds_extension( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_set_nanoseconds_extension NULL param"
  );

  /* NULL parameter */
  status = rtems_clock_set( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_set sull pointer"
  );
  puts( "TA1 - rtems_clock_set - RTEMS_INVALID_ADDRESS" );

  /* wake when NULL param */
  status = rtems_task_wake_when( NULL );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_task_wake_when NULL param"
  );
  puts( "TA1 - rtems_task_wake_when - RTEMS_INVALID_ADDRESS" );

  /* wake when before set */
  status = rtems_task_wake_when( &time );
  if ( status == RTEMS_SUCCESSFUL ) {
    puts(
     "TA1 - rtems_task_wake_when - RTEMS_NOT_DEFINED -- "
         "DID BSP SET THE TIME OF DAY?"
    );
  } else {
    fatal_directive_status(
      status,
      RTEMS_NOT_DEFINED,
      "rtems_task_wake_when before clock is set"
    );
    puts( "TA1 - rtems_task_wake_when - RTEMS_NOT_DEFINED" );
  }

  /* before time set */
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

  build_time(
    &time, 2, 5, 1988, 8, 30, 45,
    rtems_clock_get_ticks_per_second() + 1
  );
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

  build_time(
    &time,
    2, 5, 1988, 8, 30, 48,
    rtems_clock_get_ticks_per_second() + 1
  );
  time.second += 3;
  puts( "TA1 - rtems_task_wake_when - TICKINVALID - sleep about 3 seconds" );

  /* NULL time */
  status = rtems_task_wake_when( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_task_wake_when with NULL"
  );
  puts( "TA1 - rtems_task_wake_when - RTEMS_INVALID_ADDRESS" );

  /* invalid ticks */
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

  rtems_clock_get_tod( &time );
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
