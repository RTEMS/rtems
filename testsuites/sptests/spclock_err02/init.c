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

const char rtems_test_name[] = "SPCLOCK_ERR 2";

/* forward declaration to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_io_close( 0xffff, 0x0000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_close with bad major number"
  );
  puts( "TA1 - rtems_io_close - RTEMS_INVALID_NUMBER" );
  status = rtems_io_control( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_close with bad major number"
  );
  puts( "TA1 - rtems_io_control - RTEMS_INVALID_NUMBER" );
  status = rtems_io_initialize( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_initialize with bad major number"
  );
  puts( "TA1 - rtems_io_initialize - RTEMS_INVALID_NUMBER" );
  status = rtems_io_open( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_open with bad major number"
  );
  puts( "TA1 - rtems_io_open - RTEMS_INVALID_NUMBER" );
  status = rtems_io_read( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_read with bad major number"
  );
  puts( "TA1 - rtems_io_read - RTEMS_INVALID_NUMBER" );
  status = rtems_io_write( 0xffff, 0x0ffff, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_write with bad major number"
  );
  puts( "TA1 - rtems_io_write - RTEMS_INVALID_NUMBER" );

  build_time( &time, 12, 31, 2000, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 1999, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 2099, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 1, 1, 2100, 0, 0, 0, 0 );
  status = rtems_clock_set( &time );
  fatal_directive_status( status, RTEMS_INVALID_CLOCK, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_INVALID_CLOCK\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 1991, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
