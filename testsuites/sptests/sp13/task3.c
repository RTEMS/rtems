/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_3
 *
 *  This routine serves as a test task.  Its major function is to
 *  broadcast a messge to all the other tasks.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2007.
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

rtems_task Task_3(
  rtems_task_argument argument
)
{
  long              buffer[ 4 ];
  size_t            size;
  uint32_t          count;
  rtems_status_code status;

  puts(
    "TA3 - rtems_message_queue_receive - receive from queue 2 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 2 ],
    (long (*)[4])buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA3 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  Fill_buffer( "BUFFER 3 TO Q 1", (long *)buffer );
  puts( "TA3 - rtems_message_queue_broadcast - BUFFER 3 TO Q 1" );
  status = rtems_message_queue_broadcast(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
    16,
    &count
  );
  printf( "TA3 - number of tasks awakened = %02" PRIu32 "\n", count );
  puts(
    "TA3 - rtems_message_queue_receive - receive from queue 3 - "
      "RTEMS_WAIT FOREVER"
  );

  status = rtems_message_queue_receive(
    Queue_id[ 3 ],
    (long (*)[4])buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA3 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts( "TA3 - rtems_task_exit" );
  rtems_task_exit();
}
