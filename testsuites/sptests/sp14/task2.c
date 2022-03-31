/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_2
 *
 *  This routine serves as a test task.  It verifies that one task can
 *  send signals to another task ( invoking the other task's RTEMS_ASR ).
 *
 *  Input parameters:
 *    argument - task argument
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  status = rtems_task_wake_after(rtems_clock_get_ticks_per_second());
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA2 - rtems_signal_send - RTEMS_SIGNAL_17 to TA1" );
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_17 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA2 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  puts("TA2 - rtems_signal_send - RTEMS_SIGNAL_18 and RTEMS_SIGNAL_19 to TA1");
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_18 | RTEMS_SIGNAL_19 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA2 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_task_wake_after(2 * rtems_clock_get_ticks_per_second());
  directive_failed( status, "rtems_task_wake_after" );

  TEST_END();
  rtems_test_exit( 0 );
}
