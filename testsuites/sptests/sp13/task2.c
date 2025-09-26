/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_2
 *
 *  This routine serves as a test task.  Multiple tasks are required to
 *  verify all message manager capabilities.
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  (void) argument;

  long                buffer[ 4 ];
  size_t              size;
  rtems_task_priority previous_priority;
  rtems_status_code   status;

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - RTEMS_NO_WAIT"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    buffer,
    &size,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - "
      "RTEMS_WAIT FOREVER"
 );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts( "TA2 - rtems_task_set_priority - make self highest priority task" );
  status = rtems_task_set_priority( RTEMS_SELF, 3, &previous_priority );
  directive_failed( status, "rtems_task_set_priority" );

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 2 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 2 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  Fill_buffer( "BUFFER 2 TO Q 2", (long *)buffer );
  puts( "TA2 - rtems_message_queue_send - BUFFER 2 TO Q 2" );
  status = rtems_message_queue_send( Queue_id[ 2 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - "
      "10 second timeout"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    10 * rtems_clock_get_ticks_per_second()
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 3 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 3 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

}
