/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the message manager.
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

extern rtems_configuration_table BSP_Configuration;

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          qid;
  long              buffer[ 4 ];
  rtems_unsigned32  index;
  rtems_unsigned32  count;
  rtems_status_code status;

  status = rtems_message_queue_ident(
    Queue_name[ 1 ],
    RTEMS_SEARCH_ALL_NODES,
    &qid
  );
  printf( "TA1 - rtems_message_queue_ident - qid => %08x\n", qid );
  directive_failed( status, "rtems_message_queue_ident" );

  Fill_buffer( "BUFFER 1 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 2 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  Fill_buffer( "BUFFER 3 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

pause();

  Fill_buffer( "BUFFER 1 TO Q 2", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 2" );
  status = rtems_message_queue_send( Queue_id[ 2 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts_nocr( "TA1 - rtems_message_queue_receive - receive from queue 1 - " );
  puts     ( "10 second timeout" );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
    RTEMS_DEFAULT_OPTIONS,
    10 * TICKS_PER_SECOND
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA1 - buffer received: " );
  Put_buffer( buffer );
  new_line;

  puts( "TA1 - rtems_task_delete - delete TA2" );
  status = rtems_task_delete( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_delete" );

  Fill_buffer( "BUFFER 1 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

pause();

  Fill_buffer( "BUFFER 2 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 3 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 4 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 4 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 5 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_urgent - BUFFER 5 TO Q 3" );
  status = rtems_message_queue_urgent( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_urgent" );

  for ( index = 1 ; index <= 4 ; index++ ) {
    puts(
      "TA1 - rtems_message_queue_receive - receive from queue 3 - "
        "RTEMS_WAIT FOREVER"
    );
    status = rtems_message_queue_receive(
      Queue_id[ 3 ],
      (long (*)[4])buffer,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    directive_failed( status, "rtems_message_queue_receive" );
    puts_nocr( "TA1 - buffer received: " );
    Put_buffer( buffer );
    new_line;
  }

  Fill_buffer( "BUFFER 3 TO Q 2", buffer );
  puts( "TA1 - rtems_message_queue_urgent - BUFFER 3 TO Q 2" );
  status = rtems_message_queue_urgent( Queue_id[ 2 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_urgent" );

  puts(
    "TA1 - rtems_message_queue_receive - receive from queue 2 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 2 ],
    (long (*)[4])buffer,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA1 - buffer received: " );
  Put_buffer( buffer );
  new_line;

pause();

  puts( "TA1 - rtems_message_queue_delete - delete queue 1" );
  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete" );

  Fill_buffer( "BUFFER 3 TO Q 2", buffer );
  puts( "TA1 - rtems_message_queue_urgent - BUFFER 3 TO Q 2" );
  status = rtems_message_queue_urgent( Queue_id[ 2 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_urgent" );

  puts( "TA1 - rtems_message_queue_delete - delete queue 2" );
  status = rtems_message_queue_delete( Queue_id[ 2 ] );
  directive_failed( status, "rtems_message_queue_delete" );

  puts( "TA1 - rtems_message_queue_flush - empty Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

  Fill_buffer( "BUFFER 1 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 2 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 3 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_message_queue_flush - Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

  puts( "TA1 - rtems_message_queue_send until all message buffers consumed" );
  while ( FOREVER ) {
    status = rtems_message_queue_send( Queue_id[ 3 ], (long (*)[4])buffer );
    if ( status == RTEMS_UNSATISFIED ) break;
    directive_failed( status, "rtems_message_queue_send loop" );
  }

  puts( "TA1 - all message buffers consumed" );
  puts( "TA1 - rtems_message_queue_flush - Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

  if ( count != BSP_Configuration.maximum_messages )
    printf( "TA1 - ERROR - %d messages flushed!!!", count );

  puts( "*** END OF TEST 13 ***" );
  exit( 0 );
}
