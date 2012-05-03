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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_2(
  rtems_task_argument argument
)
{
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
