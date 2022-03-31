/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include "system.h"

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_event_set   eventout;
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA2 - rtems_event_receive - waiting forever on RTEMS_EVENT_16" );
  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA2 - RTEMS_EVENT_16 received - eventout => %08" PRIxrtems_event_set "\n",
     eventout
  );

  puts(
    "TA2 - rtems_event_send - send RTEMS_EVENT_14 and RTEMS_EVENT_15 to TA1"
  );
  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_14 | RTEMS_EVENT_15 );
  directive_failed( status, "rtems_event_send" );

  puts(
    "TA2 - rtems_event_receive - RTEMS_EVENT_17 or "
      "RTEMS_EVENT_18 - forever and ANY"
  );
  status = rtems_event_receive(
    RTEMS_EVENT_17 | RTEMS_EVENT_18,
    RTEMS_EVENT_ANY,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA2 - RTEMS_EVENT_17 or RTEMS_EVENT_18 received - "
      "eventout => %08" PRIxrtems_event_set "\n",
    eventout
  );

  puts( "TA2 - rtems_event_send - send RTEMS_EVENT_14 to TA1" );
  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_14 );
  directive_failed( status, "rtems_event_send" );

  build_time( &time, 2, 12, 1988, 8, 15, 0, 0 );
  print_time( "TA2 - rtems_clock_set - ", &time, "\n" );
  status = rtems_clock_set( &time );
  directive_failed( status, "TA2 rtems_clock_set" );

  time.second += 4;
  puts(
    "TA2 - rtems_event_send - sending RTEMS_EVENT_10 to self after 4 seconds"
  );
  status = rtems_timer_fire_when(
    Timer_id[ 5 ],
    &time,
    TA2_send_10_to_self,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_when after 4 seconds" );

  puts( "TA2 - rtems_event_receive - waiting forever on RTEMS_EVENT_10" );
  status = rtems_event_receive(
    RTEMS_EVENT_10,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );

  printf(
    "TA2 - RTEMS_EVENT_10 received - eventout => %08" PRIxrtems_event_set "\n",
     eventout
  );
  print_time( "TA2 - rtems_clock_get_tod - ", &time, "\n" );

  puts( "TA2 - rtems_event_receive - RTEMS_PENDING_EVENTS" );
  status = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf( "TA2 - eventout => %08" PRIxrtems_event_set "\n", eventout );

  puts( "TA2 - rtems_event_receive - RTEMS_EVENT_19 - RTEMS_NO_WAIT" );
  status = rtems_event_receive(
    RTEMS_EVENT_19,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &eventout
  );
  directive_failed( status, "rtems_event_receive" );
  printf(
    "TA2 - RTEMS_EVENT_19 received - eventout => %08" PRIxrtems_event_set "\n",
     eventout
  );

  puts( "TA2 - rtems_task_delete - deletes self" );
  status = rtems_task_delete( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_delete of TA2" );
}
