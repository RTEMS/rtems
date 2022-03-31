/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014.
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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP TASK ERROR 02";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;
  
  TEST_BEGIN();
  
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
  /* rtems_task_wake_when */
 
  build_time( &time, 2, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set successful" );
  puts( " - RTEMS_SUCCESSFUL" );

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

  TEST_END();
}
