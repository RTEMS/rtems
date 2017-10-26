/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP CLOCK ERROR 2";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_interval    interval;
  struct timeval    tv;
  rtems_status_code status;
  
  TEST_BEGIN();
 
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

  puts( "TA1 - rtems_clock_get_uptime_timeval" );
  rtems_clock_get_uptime_timeval( &tv );

  puts( "TA1 - rtems_clock_get_uptime_seconds" );
  rtems_clock_get_uptime_seconds();

  puts( "TA1 - rtems_clock_get_uptime_nanoseconds" );
  rtems_clock_get_uptime_nanoseconds();

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

  /* NULL parameter */
  status = rtems_clock_set( NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_clock_set sull pointer"
  );
  puts( "TA1 - rtems_clock_set - RTEMS_INVALID_ADDRESS" );

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

  rtems_clock_get_tod( &time );
  print_time( "TA1 - current time - ", &time, "\n" );

  TEST_END();
}
