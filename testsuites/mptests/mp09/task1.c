/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task
 *
 *  This task tests global message queue operations.  It also generates
 *  an error condition.
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

char buffer1[16] = "123456789012345";
char buffer2[16] = "abcdefghijklmno";
char buffer3[16] = "ABCDEFGHIJKLMNO";
char buffer4[16] = "PQRSTUVWXYZ(){}";

rtems_task Test_task(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  uint32_t          count;
  size_t            size;
  char              receive_buffer[16];

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Getting QID of message queue" );

  do {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  status = rtems_message_queue_ident(
    Queue_name[ 2 ],
    RTEMS_SEARCH_ALL_NODES,
    &Queue_id[ 2 ]
  );
  directive_failed( status, "rtems_message_queue_ident" );

  if ( rtems_object_get_local_node() == 2 ) {
    status = rtems_message_queue_delete( Queue_id[ 2 ] );
    fatal_directive_status(
      status,
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
      "rtems_message_queue_delete"
    );
    status = rtems_message_queue_delete( Queue_id[ 1 ] );
    fatal_directive_status(
      status,
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
      "rtems_message_queue_delete"
    );
    puts(
  "rtems_message_queue_delete correctly returned RTEMS_ILLEGAL_ON_REMOTE_OBJECT"
    );

    Send_messages();
    Receive_messages();

    puts( "Flushing remote empty queue" );
    status = rtems_message_queue_flush( Queue_id[ 1 ], &count );
    directive_failed( status, "rtems_message_queue_flush" );
    printf( "%" PRIu32 " messages were flushed on the remote queue\n", count );

    puts( "Send messages to be flushed from remote queue" );
    status = rtems_message_queue_send( Queue_id[ 1 ], buffer1, 16 );
    directive_failed( status, "rtems_message_queue_send" );

    puts( "Flushing remote queue" );
    status = rtems_message_queue_flush( Queue_id[ 1 ], &count );
    directive_failed( status, "rtems_message_queue_flush" );
    printf( "%" PRIu32 " messages were flushed on the remote queue\n", count );

    puts( "Waiting for message queue to be deleted" );
    status = rtems_message_queue_receive(
      Queue_id[ 1 ],
      receive_buffer,
      &size,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    fatal_directive_status(
      status,
      RTEMS_OBJECT_WAS_DELETED,
      "rtems_message_queue_receive"
    );
    puts( "\nGlobal message queue deleted" );
  }
  else {                   /* node == 1 */
    Receive_messages();
    Send_messages();

    puts( "Delaying for 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Deleting Message queue" );
    status = rtems_message_queue_delete( Queue_id[ 1 ] );
    directive_failed( status, "rtems_message_queue_delete" );

    status = rtems_message_queue_delete( Queue_id[ 2 ] );
    directive_failed( status, "rtems_message_queue_delete" );
  }

  puts( "*** END OF TEST 9 ***" );
  rtems_test_exit( 0 );
}
