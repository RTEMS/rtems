/*  Message_queue_task
 *
 *  This task continuously sends messages to and receives messages from
 *  a global message queue.    The message buffer is viewed as an array
 *  of two sixty-four bit counts which are incremented when a message is
 *  received.
 *
 *  Input parameters:
 *    argument - task argument
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

rtems_task Message_queue_task(
  rtems_task_argument index
)
{
  rtems_status_code  status;
  rtems_unsigned32   count;
  rtems_unsigned32   yield_count;
  rtems_unsigned32  *buffer_count;
  rtems_unsigned32  *overflow_count;
  rtems_unsigned32   size;

  Msg_buffer[ index ][0] = 0;
  Msg_buffer[ index ][1] = 0;
  Msg_buffer[ index ][2] = 0;
  Msg_buffer[ index ][3] = 0;

  puts( "Getting ID of msg queue" );
  while ( FOREVER ) {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
    if ( status == RTEMS_SUCCESSFUL )
      break;
    puts( "rtems_message_queue_ident FAILED!!" );
    rtems_task_wake_after(2);
  }

  if ( Multiprocessing_configuration.node == 1 ) {
      status = rtems_message_queue_send(
        Queue_id[ 1 ],
        (long (*)[4])Msg_buffer[ index ],
        16
      );
      directive_failed( status, "rtems_message_queue_send" );
      overflow_count = &Msg_buffer[ index ][0];
      buffer_count   = &Msg_buffer[ index ][1];
  } else {
      overflow_count = &Msg_buffer[ index ][2];
      buffer_count   = &Msg_buffer[ index ][3];
  }

  while ( Stop_Test == FALSE ) {
    yield_count = 100;

    for ( count=MESSAGE_DOT_COUNT ; Stop_Test == FALSE && count ; count-- ) {
      status = rtems_message_queue_receive(
        Queue_id[ 1 ],
        (long (*)[4])Msg_buffer[ index ],
        &size,
        RTEMS_DEFAULT_OPTIONS,
        RTEMS_NO_TIMEOUT
      );
      directive_failed( status, "rtems_message_queue_receive" );

      if ( *buffer_count == (rtems_unsigned32)0xffffffff ) {
        *buffer_count    = 0;
        *overflow_count += 1;
      } else
        *buffer_count += 1;

      status = rtems_message_queue_send(
        Queue_id[ 1 ],
        (long (*)[4])Msg_buffer[ index ],
        16
      );
      directive_failed( status, "rtems_message_queue_send" );

      if (Stop_Test == FALSE)
        if ( Multiprocessing_configuration.node == 1 && --yield_count == 0 ) {
          status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
          directive_failed( status, "rtems_task_wake_after" );

          yield_count = 100;
        }
    }
    put_dot( 'm' );
  }

  Exit_test();
}
