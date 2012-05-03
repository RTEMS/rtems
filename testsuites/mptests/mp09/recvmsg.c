/*  Receive_messages
 *
 *  This routine receives and prints three messages.
 *  an error condition.
 *
 *  Input parameters:  NONE
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

void Receive_messages()
{
 rtems_status_code status;
 uint32_t          index;
 size_t            size;
 char              receive_buffer[16];

 for ( index=1 ; index <=3 ; index++ ) {
   puts( "Receiving message ..." );
   status = rtems_message_queue_receive(
     Queue_id[ 1 ],
     receive_buffer,
     &size,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_NO_TIMEOUT
   );
   directive_failed( status, "rtems_message_queue_receive" );
   puts_nocr( "Received : ");
   puts( receive_buffer );
 }

  puts( "Receiver delaying for a second" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );
}
