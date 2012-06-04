/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "pritime.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code   sc;
  rtems_time_of_day   time;
  rtems_interval      interval;
  struct timeval      timev;

  puts( "\n\n*** TEST LEGACY RTEMS_CLOCK_GET ***" );

  puts( "Init - clock_set_time" );
  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  sc = rtems_clock_set( &time );
  directive_failed( sc, "rtems_clock_set" );

  /* NULL parameter */
  sc = rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, NULL );
  fatal_directive_status( sc, RTEMS_INVALID_ADDRESS, "null pointer" );
  puts( "TA1 - rtems_clock_get - RTEMS_INVALID_ADDRESS" );

  /* arbitrary bad value for switch */
  sc = rtems_clock_get( 0xff, &timev );
  fatal_directive_status( sc, RTEMS_INVALID_NUMBER, "bad case" );
  puts( "TA1 - rtems_clock_get - RTEMS_INVALID_NUMBER" );

  sc = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( sc, "rtems_clock_get -- TOD" );
  print_time( "Init - rtems_clock_get - ", &time, "\n" );

  sc = rtems_clock_get( RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH, &interval );
  directive_failed( sc, "rtems_clock_get -- Seconds Since Epoch" );
  printf(
    "Init - rtems_clock_get - Seconds Since Epoch = %" PRIdrtems_interval "\n",
     interval
  );

  sc = rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &interval );
  directive_failed( sc, "rtems_clock_get -- Ticks Since Boot" );
  printf(
    "Init - rtems_clock_get - Ticks Since Boot = %" PRIdrtems_interval "\n",
     interval
  );

  sc = rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, &interval );
  directive_failed( sc, "rtems_clock_get -- Ticks Per Second" );
  printf(
    "Init - rtems_clock_get - Ticks Per Second = %" PRIdrtems_interval "\n",
     interval
  );

  sc = rtems_clock_get( RTEMS_CLOCK_GET_TIME_VALUE, &timev );
  directive_failed( sc, "rtems_clock_get -- Time Value" );
  printf(
    "Init - rtems_clock_get - Time Value = %" PRIdtime_t "\n",
     timev.tv_sec
  );

  puts( "*** END OF TEST LEGACY RTEMS_CLOCK_GET ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
