/*  Send_messages
 *
 *  This routine sends a series of three messages.
 *  an error condition.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
