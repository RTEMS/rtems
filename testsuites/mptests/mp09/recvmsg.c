/*  Receive_messages
 *
 *  This routine receives and prints three messages.
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

void Receive_messages()
{
 rtems_status_code status;
 rtems_unsigned32  index;
 char              receive_buffer[16];

 for ( index=1 ; index <=3 ; index++ ) {
   puts( "Receiving message ..." );
   status = rtems_message_queue_receive(
     Queue_id[ 1 ],
     (long (*)[4])receive_buffer,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT
   );
   directive_failed( status, "rtems_message_queue_receive" );
   puts_nocr( "Received : ");
   puts( receive_buffer );
 }

  puts( "Receiver delaying for a second" );
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );
}
