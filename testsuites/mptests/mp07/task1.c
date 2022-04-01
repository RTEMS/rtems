/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task continuously sends an event to its counterpart on the
 *  other node, and then waits for it to send an event.  The copy
 *  running on node one send the first event.
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

#define DOT_COUNT 100

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
  uint32_t    count;
  uint32_t    remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;

  Stop_Test = false;

  remote_node = (rtems_object_get_local_node() == 1) ? 2 : 1;
  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  do {
    status = rtems_task_ident(
      Task_name[ remote_node ],
      RTEMS_SEARCH_ALL_NODES,
      &remote_tid
    );
  } while ( !rtems_is_status_successful( status ) );

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Sending first event to remote task" );
    status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
    directive_failed( status, "rtems_event_send" );
  }

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    5 * rtems_clock_get_ticks_per_second(),
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  while ( true ) {
    for ( count=DOT_COUNT ; count && (Stop_Test == false) ; count-- ) {
      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_DEFAULT_OPTIONS,
        rtems_clock_get_ticks_per_second(),
        &event_out
      );
      if ( status == RTEMS_TIMEOUT ) {
        printf("\nTA1 - RTEMS_TIMEOUT .. probably OK if the other node exits");
        Stop_Test = true;
        break;
      } else
        directive_failed( status, "rtems_event_receive" );

      status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
      directive_failed( status, "rtems_event_send" );
    }
    if ( Stop_Test )
       break;
    put_dot('.');
  }

  /*
   * Wait a bit before shutting down so we don't screw up the other node
   * when our MPCI shuts down
   */

  rtems_task_wake_after(10);

  puts( "\n*** END OF TEST 7 ***" );
  rtems_test_exit( 0 );
}
