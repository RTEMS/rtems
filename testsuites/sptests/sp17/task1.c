/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This task initializes the signal catcher, sends the first signal
 *  if running on the first node, and loops while waiting for signals.
 *
 *  NOTE: The signal catcher is not reentrant and hence RTEMS_NO_ASR must
 *        be a part of its execution mode.
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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA1 - rtems_signal_catch: initializing signal catcher" );
  status = rtems_signal_catch( Process_asr, RTEMS_NO_ASR | RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  puts( "TA1 - Sending signal to self" );
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_16 );
  directive_failed( status, "rtems_signal_send" );

  if ( Task_2_preempted == TRUE )
    puts( "TA1 - TA2 correctly preempted me" );

  puts("TA1 - Got Back!!!");

  TEST_END();
  rtems_test_exit( 0 );
}
