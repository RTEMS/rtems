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

rtems_task Test_task1(
  rtems_task_argument argument
)
{
  char              receive_buffer[16];
  rtems_unsigned32  size;
  rtems_status_code status;

  puts( "Getting QID of message queue" );

  do {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Receiving message ..." );
    status = rtems_message_queue_receive(
      Queue_id[ 1 ],
      (long (*)[4])receive_buffer,
      &size,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    puts( "How did I get back from here????" );
    directive_failed( status, "rtems_message_queue_receive" );
  }

  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Receiving message ..." );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])receive_buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    2 * TICKS_PER_SECOND
  );
  fatal_directive_status(status, RTEMS_TIMEOUT, "rtems_message_queue_receive");
  puts( "rtems_message_queue_receive correctly returned RTEMS_TIMEOUT" );

  puts( "Deleting self" );
  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
