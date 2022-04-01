/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
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
 *  COPYRIGHT (c) 1989-2009.
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

#define SIGNALS_PER_DOT 15

static rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Stop_Test = true;
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Stop_Test = false;

  signal_caught = 0;
  signal_count  = 0;

  puts( "rtems_signal_catch: initializing signal catcher" );
  status = rtems_signal_catch( Process_asr, RTEMS_NO_ASR|RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  if (rtems_object_get_local_node() == 1) {
     remote_node = 2;
     remote_signal  = RTEMS_SIGNAL_18;
     expected_signal = RTEMS_SIGNAL_17;
  }
  else {
     remote_node = 1;
     remote_signal  = RTEMS_SIGNAL_17;
     expected_signal = RTEMS_SIGNAL_18;
  }
  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  do {
      status = rtems_task_ident(
          Task_name[ remote_node ],
          RTEMS_SEARCH_ALL_NODES,
          &remote_tid
          );
  } while ( status != RTEMS_SUCCESSFUL );
  directive_failed( status, "rtems_task_ident" );

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    3 * rtems_clock_get_ticks_per_second(),
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Sending signal to remote task" );
    do {
      status = rtems_signal_send( remote_tid, remote_signal );
      if ( status == RTEMS_NOT_DEFINED )
        continue;
    } while ( status != RTEMS_SUCCESSFUL );
    directive_failed( status, "rtems_signal_send" );
  }

  while ( Stop_Test == FALSE ) {
    if ( signal_caught ) {
      signal_caught = 0;
      if ( ++signal_count >= SIGNALS_PER_DOT ) {
        signal_count = 0;
        put_dot( '.' );
      }
      status = rtems_signal_send( remote_tid, remote_signal );
      directive_failed( status, "rtems_signal_send" );
    }
  }
  puts( "\n*** END OF TEST 5 ***" );
  rtems_test_exit( 0 );
}
