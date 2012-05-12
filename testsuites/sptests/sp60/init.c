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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  rtems_id              period1;
  rtems_id              period2;

  puts( "\n\n*** TEST 60 ***" );

  puts( "Init - rtems_rate_monotonic_create - first period" );
  sc = rtems_rate_monotonic_create(
    rtems_build_name( 'P', 'E', 'R', '1' ),
    &period1
  );
  directive_failed( sc, "rtems_rate_monotonic_create 1" );

  puts( "Init - rtems_rate_monotonic_create - second period" );
  sc = rtems_rate_monotonic_create(
    rtems_build_name( 'P', 'E', 'R', '2' ),
    &period2
  );
  directive_failed( sc, "rtems_rate_monotonic_create 1" );

  puts( "Init - rtems_rate_monotonic_period - short period" );
  sc = rtems_rate_monotonic_period(period1, RTEMS_MILLISECONDS_TO_TICKS(200) );
  directive_failed( sc, "rtems_rate_monotonic_period" );

  puts( "Init - rtems_rate_monotonic_period - long period initiated" );
  sc = rtems_rate_monotonic_period(period2, RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( sc, "rtems_rate_monotonic_period" );

  puts( "Init - rtems_rate_monotonic_period - long period block" );
  sc = rtems_rate_monotonic_period(period2, RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( sc, "rtems_rate_monotonic_period" );

  puts( "Init - rtems_rate_monotonic_period - verify long period expired" );
  sc = rtems_rate_monotonic_period(period1, RTEMS_PERIOD_STATUS );
  fatal_directive_status(sc, RTEMS_TIMEOUT, "rtems_task_period status");

  puts( "*** END OF TEST 60 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_PERIODS       2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
