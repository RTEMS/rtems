/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "SPTIMER_ERR 1";

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
  (void) argument;

  TEST_BEGIN();

  rtems_status_code        status;
  rtems_time_of_day        time;
  rtems_timer_information  timer_info;
  rtems_name               timer_name;
  rtems_id                 timer_id;
  rtems_id                 junk_id;

  timer_name =  rtems_build_name( 'T', 'M', '1', ' ' );

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

  /* Set System time */
  build_time( &time, 12, 31, 1992, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  /* NULL Id */
  status = rtems_timer_create( timer_name, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_create NULL param"
  );
  puts( "TA1 - rtems_timer_create - RTEMS_INVALID_ADDRESS" );

  /* bad name */
  status = rtems_timer_create( 0, &junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_timer_create with illegal name"
  );
  puts( "TA1 - rtems_timer_create - RTEMS_INVALID_NAME" );

  /* OK */
  status = rtems_timer_create( timer_name, &timer_id );
  directive_failed( status, "rtems_timer_create" );
  puts( "TA1 - rtems_timer_create - 1 - RTEMS_SUCCESSFUL" );

  status = rtems_timer_create( 2, &junk_id );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_timer_create for too many"
  );
  puts( "TA1 - rtems_timer_create - 2 - RTEMS_TOO_MANY" );

  status = rtems_timer_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_delete with illegal id"
  );
  puts( "TA1 - rtems_timer_delete - local RTEMS_INVALID_ID" );

  status = rtems_timer_delete( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_delete with illegal id"
  );
  puts( "TA1 - rtems_timer_delete - global RTEMS_INVALID_ID" );

  status = rtems_timer_ident( 0, &junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_timer_ident with illegal name"
  );
  puts( "TA1 - rtems_timer_ident - RTEMS_INVALID_NAME" );

  status = rtems_timer_cancel( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_cancel with illegal id"
  );
  puts( "TA1 - rtems_timer_cancel - RTEMS_INVALID_ID" );

  status = rtems_timer_reset( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_reset with illegal id"
  );
  puts( "TA1 - rtems_timer_reset - RTEMS_INVALID_ID" );

  status = rtems_timer_reset( timer_id );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_timer_reset before initiated"
  );
  puts( "TA1 - rtems_timer_reset - RTEMS_NOT_DEFINED" );

  /* bad id */
  status = rtems_timer_fire_after(
    rtems_build_id( 1, 1, 1, 256 ),
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_ID" );

  /* bad id */
  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_fire_when(
    rtems_build_id( 1, 1, 1, 256 ),
    &time,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_fire_when - RTEMS_INVALID_ID" );

  /* NULL routine */
  status = rtems_timer_fire_after( timer_id, 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_fire_after with NULL handler"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_ADDRESS" );

  /* 0 ticks */
  status = rtems_timer_fire_after( timer_id, 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_NUMBER" );

  /* NULL routine */
  status = rtems_timer_fire_when( timer_id, &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_fire_when with NULL handler"
  );
  puts( "TA1 - rtems_timer_fire_when - RTEMS_INVALID_ADDRESS" );

  /* invalid time -- before RTEMS epoch */
  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  status = rtems_timer_fire_when( timer_id, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_fire_when with illegal time"
  );
  print_time(
    "TA1 - rtems_timer_fire_when - ",
    &time,
    " - RTEMS_INVALID_CLOCK\n"
  );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod       - ", &time, "\n" );

  build_time( &time, 2, 5, 1990, 8, 30, 45, 0 );
  status = rtems_timer_fire_when( timer_id, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_fire_when before current time"
  );
  print_time(
    "TA1 - rtems_timer_fire_when - ",
    &time,
    " - before RTEMS_INVALID_CLOCK\n"
  );

  /* null param */
  status = rtems_timer_get_information( timer_id, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_get_information with NULL param"
  );
  puts( "TA1 - rtems_timer_get_information - RTEMS_INVALID_ADDRESS" );

  /* invalid id */
  status = rtems_timer_get_information( 100, &timer_info );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_get_information with illegal id"
  );
  puts( "TA1 - rtems_timer_get_information - RTEMS_INVALID_ID" );


  TEST_END();

  rtems_test_exit(0);
}

rtems_timer_service_routine Delayed_routine(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;

  /* Empty routine that gets passed to rtems_timer_fire_when */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TIMERS              1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
