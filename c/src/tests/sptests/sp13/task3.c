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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  task3.c,v 1.2 1995/05/31 17:10:24 joel Exp
 */

#include "system.h"

rtems_task Task_3(
  rtems_task_argument argument
)
{
  long              buffer[ 4 ];
  rtems_unsigned32  size;
  rtems_unsigned32  count;
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
  printf( "TA3 - number of tasks awakened = %02d\n", count );
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

  puts( "TA3 - rtems_task_delete - delete self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
