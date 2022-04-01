/* SPDX-License-Identifier: BSD-2-Clause */

/*  Message_queue_task
 *
 *  This task continuously sends messages to and receives messages from
 *  a global message queue.    The message buffer is viewed as an array
 *  of two sixty-four bit counts which are incremented when a message is
 *  received.
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

rtems_task Message_queue_task(
  rtems_task_argument index
)
{
  rtems_status_code  status;
  uint32_t     count;
  uint32_t     yield_count;
  uint32_t    *buffer_count;
  uint32_t    *overflow_count;
  size_t       size;

  Msg_buffer[ index ][0] = 0;
  Msg_buffer[ index ][1] = 0;
  Msg_buffer[ index ][2] = 0;
  Msg_buffer[ index ][3] = 0;

  puts( "Getting ID of msg queue" );
  while ( FOREVER ) {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
    if ( status == RTEMS_SUCCESSFUL )
      break;
    puts( "rtems_message_queue_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  if ( rtems_object_get_local_node() == 1 ) {
      status = rtems_message_queue_send(
        Queue_id[ 1 ],
        (long (*)[4])Msg_buffer[ index ],
        16
      );
      directive_failed( status, "rtems_message_queue_send" );
      overflow_count = &Msg_buffer[ index ][0];
      buffer_count   = &Msg_buffer[ index ][1];
  } else {
      overflow_count = &Msg_buffer[ index ][2];
      buffer_count   = &Msg_buffer[ index ][3];
  }

  while ( Stop_Test == false ) {
    yield_count = 100;

    for ( count=MESSAGE_DOT_COUNT ; Stop_Test == false && count ; count-- ) {
      status = rtems_message_queue_receive(
        Queue_id[ 1 ],
        Msg_buffer[ index ],
        &size,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT
      );
      directive_failed( status, "rtems_message_queue_receive" );

      if ( *buffer_count == (uint32_t)0xffffffff ) {
        *buffer_count    = 0;
        *overflow_count += 1;
      } else
        *buffer_count += 1;

      status = rtems_message_queue_send(
        Queue_id[ 1 ],
        Msg_buffer[ index ],
        16
      );
      directive_failed( status, "rtems_message_queue_send" );

      if (Stop_Test == false)
        if ( rtems_object_get_local_node() == 1 && --yield_count == 0 ) {
          status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
          directive_failed( status, "rtems_task_wake_after" );

          yield_count = 100;
        }
    }
    put_dot( 'm' );
  }

  Exit_test();
}
