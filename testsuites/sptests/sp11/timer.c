/* SPDX-License-Identifier: BSD-2-Clause */

/*  Timer_functions
 *
 *  These routines are the timer service routines used by this test.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

rtems_timer_service_routine TA1_send_18_to_self_5_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_18 );
  directive_failed_with_level( status, "rtems_event_send of 18", 1 );
}

rtems_timer_service_routine TA1_send_8_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_8 );
  directive_failed_with_level( status, "rtems_event_send of 8", 1 );
}

rtems_timer_service_routine TA1_send_9_to_self_60_seconds(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_9 );
  directive_failed_with_level( status, "rtems_event_send of 9", 1 );
}

rtems_timer_service_routine TA1_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_10 );
  directive_failed_with_level( status, "rtems_event_send of 10", -1 );
}

rtems_timer_service_routine TA1_send_1_to_self_every_second(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_1 );
  directive_failed_with_level( status, "rtems_event_send of 1", 1 );
}

rtems_timer_service_routine TA1_send_11_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 1 ], RTEMS_EVENT_11 );
  directive_failed_with_level( status, "rtems_event_send of 11", -1 );
}

rtems_timer_service_routine TA2_send_10_to_self(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code status;

  status = rtems_event_send( Task_id[ 2 ], RTEMS_EVENT_10 );
  directive_failed_with_level( status, "rtems_event_send of 10", 1 );
}
