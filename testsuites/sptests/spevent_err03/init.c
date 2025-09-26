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

const char rtems_test_name[] = "SP EVENT ERROR 3";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_event_set   event_out;
  rtems_time_of_day time;
  struct timeval    tv;
  time_t            seconds;
  rtems_status_code status;
  
  TEST_BEGIN();
  
  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_event_receive NULL param"
  );
  puts( "TA1 - rtems_event_receive - NULL param - RTEMS_INVALID_ADDRESS" );

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ALL)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( all conditions )" );

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_EVENT_ANY | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ANY)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( any condition )" );

  puts( "TA1 - rtems_event_receive - timeout in 3 seconds" );
  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second(),
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_event_receive"
  );
  puts( "TA1 - rtems_event_receive - woke up with RTEMS_TIMEOUT" );

  status = rtems_event_send( 100, RTEMS_EVENT_16 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_event_send with illegal id"
  );
  puts( "TA1 - rtems_event_send - RTEMS_INVALID_ID" );

  puts( "TA1 - rtems_task_wake_after - sleep 1 second - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  build_time( &time, 2, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  puts( " - RTEMS_SUCCESSFUL" );

  status = rtems_clock_get_tod_timeval( &tv );
  directive_failed( status, "clock_get_tod_timeval OK" );

  seconds = tv.tv_sec;
  printf( "TA1 - current time - %s\n", ctime(&seconds) );

  TEST_END();
}
