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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "system.h"

rtems_task Task_2(
  rtems_task_argument argument
)
{
  long                buffer[ 4 ];
  rtems_task_priority previous_priority;
  rtems_status_code   status;

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - RTEMS_NO_WAIT"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
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
    (long (*)[4])buffer,
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
    (long (*)[4])buffer,
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
    (long (*)[4])buffer,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  Fill_buffer( "BUFFER 2 TO Q 2", (long *)buffer );
  puts( "TA2 - rtems_message_queue_send - BUFFER 2 TO Q 2" );
  directive_failed( status, "rtems_message_queue_send" );

  status = rtems_message_queue_send( Queue_id[ 2 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts(
    "TA2 - rtems_message_queue_receive - receive from queue 1 - "
      "10 second timeout"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
    RTEMS_DEFAULT_OPTIONS,
    10 * TICKS_PER_SECOND
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
    (long (*)[4])buffer,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA2 - buffer received: " );
  Put_buffer( buffer );
  new_line;

}
