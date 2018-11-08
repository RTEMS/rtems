/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "SPTIMER_ERR 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  rtems_status_code        status;
  rtems_time_of_day        time;
  rtems_id                 timer_id;
  rtems_name               timer_name;

  /* Set System time */
  build_time( &time, 12, 31, 1992, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  timer_name =  rtems_build_name( 'T', 'M', '1', ' ' );

  /* OK */
  status = rtems_timer_create( timer_name, &timer_id );
  directive_failed( status, "rtems_timer_create" );
  puts( "TA1 - rtems_timer_create - 1 - RTEMS_SUCCESSFUL" );

  /* incorrect state */
  status = rtems_timer_server_fire_after( 0, 5, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_after incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INCORRECT_STATE" );

  /* incorrect state */
  status = rtems_timer_server_fire_when( 0, &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_when incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INCORRECT_STATE" );

  /* invalid priority */
  status = rtems_timer_initiate_server( UINT32_MAX - 1, 0, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_timer_initiate_server invalid priority"
  );
  puts( "TA1 - rtems_timer_initiate_server - RTEMS_INVALID_PRIORITY" );

  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    0x10000000,
    0
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_timer_initiate_server too much stack "
  );
  puts( "TA1 - rtems_timer_initiate_server - RTEMS_UNSATISFIED" );

  status =
    rtems_timer_initiate_server( RTEMS_TIMER_SERVER_DEFAULT_PRIORITY, 0, 0 );
  directive_failed( status, "rtems_timer_initiate_server" );
  puts( "TA1 - rtems_timer_initiate_server - SUCCESSFUL" );

  /* NULL routine */
  status = rtems_timer_server_fire_after( timer_id, 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_after NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ADDRESS" );

  /* bad Id */
  status = rtems_timer_server_fire_after(
    rtems_build_id( 1, 1, 1, 256 ),
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ID" );

  /* bad id */
  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_server_fire_when(
    rtems_build_id( 1, 1, 1, 256 ),
    &time,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INVALID_ID" );

  /* NULL routine */
  status = rtems_timer_server_fire_after( timer_id, 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_after NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ADDRESS" );

  /* 0 ticks */
  status = rtems_timer_server_fire_after(
    timer_id, 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_server_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_NUMBER" );

  /* illegal time */
  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when(
    timer_id, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_server_fire_when with illegal time"
  );
  print_time(
    "TA1 - rtems_timer_server_fire_when - ",
    &time,
    " - RTEMS_INVALID_CLOCK\n"
  );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod       - ", &time, "\n" );

  /* when NULL routine */
  status = rtems_timer_server_fire_when( timer_id, &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_when NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INVALID_ADDRESS" );

  /* before current time */
  build_time( &time, 2, 5, 1990, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when(
    timer_id, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_server_fire_when before current time"
  );
  print_time(
    "TA1 - rtems_timer_server_fire_when - ",
    &time,
    " - before RTEMS_INVALID_CLOCK\n"
  );

  TEST_END();

  rtems_test_exit(0);
}

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  /* Empty routine that gets passed to rtems_timer_fire_when */
}

#define CONFIGURE_INIT
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Two Tasks: Init and Timer Server */
#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_TIMERS          1
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>
