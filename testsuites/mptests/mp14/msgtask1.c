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

rtems_task Message_queue_task(
  rtems_task_argument index
)
{
  rtems_status_code  status;
  rtems_unsigned32   count;
  rtems_unsigned32   yield_count;
  rtems_unsigned32  *buffer_count;
  rtems_unsigned32  *overflow_count;

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
  }

  if ( Multiprocessing_configuration.node == 1 ) {
      status = rtems_message_queue_send(
        Queue_id[ 1 ],
        (long (*)[4])Msg_buffer[ index ]
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
        (long (*)[4])Msg_buffer[ index ]
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
