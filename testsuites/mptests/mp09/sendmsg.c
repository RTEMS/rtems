/*  Send_messages
 *
 *  This routine sends a series of three messages.
 *  an error condition.
 *
 *  Input parameters:  NONE
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

void Send_messages()
{
  rtems_status_code status;
  rtems_unsigned32  broadcast_count;

  puts_nocr( "rtems_message_queue_send: " );
  puts( buffer1 );

  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4])buffer1, 16 );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "Delaying for a second" );
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts_nocr( "rtems_message_queue_urgent: " );
  puts( buffer2 );
  status = rtems_message_queue_urgent( Queue_id[ 1 ], (long (*)[4])buffer2, 16 );
  directive_failed( status, "rtems_message_queue_urgent" );

  puts( "Delaying for a second" );
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts_nocr( "rtems_message_queue_broadcast: " );
  puts( buffer3 );
  status = rtems_message_queue_broadcast(
    Queue_id[ 1 ],
    (long (*)[4])buffer3,
    16,
    &broadcast_count
  );
  directive_failed( status, "rtems_message_queue_broadcast" );

  puts( "Delaying for a second" );
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );
}
