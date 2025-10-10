/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test_task1
 *
 *  This task attempts to receive a message from a global message queue.
 *  If running on the node on which the queue resides, the wait is
 *  forever, otherwise it times out on a remote message queue.
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

rtems_task Test_task1(
  rtems_task_argument argument
)
{
  (void) argument;

  char              receive_buffer[16];
  size_t            size;
  rtems_status_code status;

  puts( "Getting QID of message queue" );

  do {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Receiving message ..." );
    status = rtems_message_queue_receive(
      Queue_id[ 1 ],
      receive_buffer,
      &size,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    puts( "How did I get back from here????" );
    directive_failed( status, "rtems_message_queue_receive" );
  }

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Receiving message ..." );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])receive_buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    2 * rtems_clock_get_ticks_per_second()
  );
  fatal_directive_status(status, RTEMS_TIMEOUT, "rtems_message_queue_receive");
  puts( "rtems_message_queue_receive correctly returned RTEMS_TIMEOUT" );

  puts( "Deleting self" );
  rtems_task_exit();
}
