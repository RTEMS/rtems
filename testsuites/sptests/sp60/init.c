/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

const char rtems_test_name[] = "SP 60";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  rtems_id              period1;
  rtems_id              period2;

  TEST_BEGIN();

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

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_PERIODS       2
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
