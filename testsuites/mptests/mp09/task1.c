/*  Test_task
 *
 *  This task tests global message queue operations.  It also generates
 *  an error condition.
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

char buffer1[16] = "123456789012345";
char buffer2[16] = "abcdefghijklmno";
char buffer3[16] = "ABCDEFGHIJKLMNO";
char buffer4[16] = "PQRSTUVWXYZ(){}";

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t          count;
  size_t            size;
  char              receive_buffer[16];

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Getting QID of message queue" );

  do {
    status = rtems_message_queue_ident(
      Queue_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Queue_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  if ( Multiprocessing_configuration.node == 2 ) {
    status = rtems_message_queue_delete( Queue_id[ 1 ] );
    fatal_directive_status(
      status,
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
      "rtems_message_queue_delete"
    );
    puts(
  "rtems_message_queue_delete correctly returned RTEMS_ILLEGAL_ON_REMOTE_OBJECT"
    );

    Send_messages();
    Receive_messages();

    puts( "Flushing remote empty queue" );
    status = rtems_message_queue_flush( Queue_id[ 1 ], &count );
    directive_failed( status, "rtems_message_queue_flush" );
    printf( "%" PRIu32 " messages were flushed on the remote queue\n", count );

    puts( "Send messages to be flushed from remote queue" );
    status = rtems_message_queue_send( Queue_id[ 1 ], buffer1, 16 );
    directive_failed( status, "rtems_message_queue_send" );

    puts( "Flushing remote queue" );
    status = rtems_message_queue_flush( Queue_id[ 1 ], &count );
    directive_failed( status, "rtems_message_queue_flush" );
    printf( "%" PRIu32 " messages were flushed on the remote queue\n", count );

    puts( "Waiting for message queue to be deleted" );
    status = rtems_message_queue_receive(
      Queue_id[ 1 ],
      receive_buffer,
      &size,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    fatal_directive_status(
      status,
      RTEMS_OBJECT_WAS_DELETED,
      "rtems_message_queue_receive"
    );
    puts( "\nGlobal message queue deleted" );
  }
  else {                   /* node == 1 */
    Receive_messages();
    Send_messages();

    puts( "Delaying for 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Deleting Message queue" );
    status = rtems_message_queue_delete( Queue_id[ 1 ] );
    directive_failed( status, "rtems_message_queue_delete" );
  }

  puts( "*** END OF TEST 9 ***" );
  rtems_test_exit( 0 );
}
