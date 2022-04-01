/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task either continuously sends events to a remote task, or
 *  continuously receives events sent by a remote task.  This decision
 *  is based upon the local node number.
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

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t    count;
  uint32_t    remote_node;
  rtems_id          remote_tid;
  rtems_event_set   event_out;

  remote_node = ((rtems_object_get_local_node() == 1) ? 2 : 1);

  puts( "About to go to sleep!" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );
  puts( "Waking up!" );

  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  while ( FOREVER ) {
    status = rtems_task_ident(
      Task_name[ remote_node ],
      RTEMS_SEARCH_ALL_NODES,
      &remote_tid
    );

    if ( status == RTEMS_SUCCESSFUL )
      break;
    puts( "rtems_task_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Sending events to remote task" );
    while ( Stop_Test == false ) {
      for ( count=EVENT_TASK_DOT_COUNT; Stop_Test == false && count; count-- ) {
        status = rtems_event_send( remote_tid, RTEMS_EVENT_16 );
        directive_failed( status, "rtems_event_send" );
     }
     put_dot( 'e' );
    }
  }

  puts( "Receiving events from remote task" );
  while ( Stop_Test == false ) {
    for ( count=EVENT_TASK_DOT_COUNT ; Stop_Test == false && count ; count-- ) {
      status = rtems_event_receive(
        RTEMS_EVENT_16,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT,
        &event_out
      );
      directive_failed( status, "rtems_event_receive" );
    }
    put_dot( 'e' );
  }

  Exit_test();
}
