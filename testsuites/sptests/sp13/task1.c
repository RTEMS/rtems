/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the message manager.
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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"
#include <string.h>             /* for memcmp */

char    big_send_buffer[2048];
char    big_receive_buffer[2048];

long    buffer[ 4 ];

#define MESSAGE_SIZE (sizeof(long) * 4)

void dope_buffer(unsigned char *buff,
                 int   buff_size,
                 unsigned32 v)
{
    int i;
    unsigned char ch;

    ch = (' ' + (v % (0x7f - ' ')));
    
    for (i=0; i<buff_size; i++)
    {
        *buff++ = ch++;
        if (ch >= 0x7f)
            ch = ' ';
    }
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          qid;
  rtems_unsigned32  index;
  rtems_unsigned32  count;
  rtems_status_code status;
  rtems_unsigned32  size;
  rtems_unsigned32  queue_size;
  char             *cp;
  
  status = rtems_message_queue_ident(
    Queue_name[ 1 ],
    RTEMS_SEARCH_ALL_NODES,
    &qid
  );
  printf( "TA1 - rtems_message_queue_ident - qid => %08x\n", qid );
  directive_failed( status, "rtems_message_queue_ident" );

  Fill_buffer( "BUFFER 1 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 2 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  Fill_buffer( "BUFFER 3 TO Q 1", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1" );
  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

rtems_test_pause();

  Fill_buffer( "BUFFER 1 TO Q 2", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 2" );
  status = rtems_message_queue_send( Queue_id[ 2 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts_nocr( "TA1 - rtems_message_queue_receive - receive from queue 1 - " );
  puts     ( "10 second timeout" );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    buffer,
    &size,
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
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

rtems_test_pause();

  Fill_buffer( "BUFFER 2 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 3 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 4 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 4 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 5 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_urgent - BUFFER 5 TO Q 3" );
  status = rtems_message_queue_urgent( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_urgent" );

  for ( index = 1 ; index <= 4 ; index++ ) {
    puts(
      "TA1 - rtems_message_queue_receive - receive from queue 3 - "
        "RTEMS_WAIT FOREVER"
    );
    status = rtems_message_queue_receive(
      Queue_id[ 3 ],
      buffer,
      &size,
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
  status = rtems_message_queue_urgent( Queue_id[ 2 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_urgent" );

  puts(
    "TA1 - rtems_message_queue_receive - receive from queue 2 - "
      "RTEMS_WAIT FOREVER"
  );
  status = rtems_message_queue_receive(
    Queue_id[ 2 ],
    buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_message_queue_receive" );
  puts_nocr( "TA1 - buffer received: " );
  Put_buffer( buffer );
  new_line;

rtems_test_pause();

  puts( "TA1 - rtems_message_queue_delete - delete queue 1" );
  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete" );

  Fill_buffer( "BUFFER 3 TO Q 2", buffer );
  puts( "TA1 - rtems_message_queue_urgent - BUFFER 3 TO Q 2" );
  status = rtems_message_queue_urgent( Queue_id[ 2 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_urgent" );

  puts( "TA1 - rtems_message_queue_delete - delete queue 2" );
  status = rtems_message_queue_delete( Queue_id[ 2 ] );
  directive_failed( status, "rtems_message_queue_delete" );

  puts( "TA1 - rtems_message_queue_get_number_pending - SUCCESSFUL\n" );
  status = rtems_message_queue_get_number_pending( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages are pending on Q 3\n", count );

  puts( "TA1 - rtems_message_queue_flush - empty Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

  Fill_buffer( "BUFFER 1 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  Fill_buffer( "BUFFER 2 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  /* this broadcast should have no effect on the queue */
  Fill_buffer( "NO BUFFER TO Q1", (long *)buffer );
  puts( "TA3 - rtems_message_queue_broadcast - NO BUFFER TO Q1" );
  status = rtems_message_queue_broadcast(
    Queue_id[ 1 ],
    (long (*)[4])buffer,
    16,
    &count
  );
  printf( "TA3 - number of tasks awakened = %d\n", count );


  puts( "TA1 - rtems_message_queue_get_number_pending - SUCCESSFUL\n" );
  status = rtems_message_queue_get_number_pending( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages are pending on Q 3\n", count );

  Fill_buffer( "BUFFER 3 TO Q 3", buffer );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 3" );
  status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );

  puts( "TA1 - rtems_message_queue_flush - Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

  puts( "TA1 - rtems_message_queue_send until all message buffers consumed" );
  while ( FOREVER ) {
    status = rtems_message_queue_send( Queue_id[ 3 ], buffer, MESSAGE_SIZE );
    if ( status == RTEMS_TOO_MANY ) break;
    directive_failed( status, "rtems_message_queue_send loop" );
  }

  puts( "TA1 - all message buffers consumed" );
  puts( "TA1 - rtems_message_queue_flush - Q 3" );
  status = rtems_message_queue_flush( Queue_id[ 3 ], &count );
  printf( "TA1 - %d messages were flushed from Q 3\n", count );

rtems_test_pause();

  puts( "TA1 - create message queue of 20 bytes on queue 1" );
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    100,
    20,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create of Q1; 20 bytes each" );
  status = rtems_message_queue_send( Queue_id[ 3 ], big_send_buffer, 40 );
  fatal_directive_status(status, RTEMS_INVALID_SIZE, "expected RTEMS_INVALID_SIZE");
  
  puts( "TA1 - rtems_message_queue_delete - delete queue 1" );
  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete" );

rtems_test_pause();
  
  puts( "TA1 - rtems_message_queue_create - variable sizes " );
  for (queue_size = 1; queue_size < 1030; queue_size++)
  {
      status = rtems_message_queue_create(
          Queue_name[ 1 ],
          2,            /* just 2 msgs each */
          queue_size,
          RTEMS_DEFAULT_ATTRIBUTES,
          &Queue_id[ 1 ]
          );
      if (status != RTEMS_SUCCESSFUL)
      {
          printf("TA1 - msq que size: %d\n", queue_size);
          directive_failed( status, "rtems_message_queue_create of Q1" );
      }

      status = rtems_message_queue_delete( Queue_id[ 1 ] );
      directive_failed( status, "rtems_message_queue_delete" );
  }
  
  puts( "TA1 - rtems_message_queue_create and send - variable sizes " );
  for (queue_size = 1; queue_size < 1030; queue_size++)
  {
      /* printf("TA1 - message queue size: %d\n", queue_size); */

      status = rtems_message_queue_create(
          Queue_name[ 1 ],
          2,            /* just 2 msgs each */
          queue_size,
          RTEMS_DEFAULT_ATTRIBUTES,
          &Queue_id[ 1 ]
          );

      directive_failed( status, "rtems_message_queue_create of Q1" );

      dope_buffer(big_send_buffer, sizeof(big_send_buffer), queue_size);
      memset(big_receive_buffer, 'Z', sizeof(big_receive_buffer));

      /* send a msg too big */
      status = rtems_message_queue_send( Queue_id[ 1 ],
                                         big_send_buffer,
                                         queue_size + 1 );

      fatal_directive_status(status,
                             RTEMS_INVALID_SIZE,
                             "rtems_message_queue_send too large");

      /* send a msg that is just right */
      status = rtems_message_queue_send(Queue_id[ 1 ],
                                        big_send_buffer,
                                        queue_size);
      directive_failed(status, "rtems_message_queue_send exact size");

      /* now read and verify the message just sent */
      status = rtems_message_queue_receive(Queue_id[ 1 ],
                                           big_receive_buffer,
                                           &size,
                                           RTEMS_DEFAULT_OPTIONS,
                                           1 * TICKS_PER_SECOND);
      directive_failed(status, "rtems_message_queue_receive exact size");
      if (size != queue_size)
      {
          puts("TA1 - exact size size match failed");
          rtems_test_exit(1);
      }

      if (memcmp(big_send_buffer, big_receive_buffer, size) != 0)
      {
          puts("TA1 - exact size data match failed");
          rtems_test_exit(1);
      }

      for (cp = (big_receive_buffer + size);
           cp < (big_receive_buffer + sizeof(big_receive_buffer));
           cp++)
          if (*cp != 'Z')
          {
              puts("TA1 - exact size overrun match failed");
              rtems_test_exit(1);
          }
          
      /* all done with this one; delete it */
      status = rtems_message_queue_delete( Queue_id[ 1 ] );
      directive_failed( status, "rtems_message_queue_delete" );
  }
  
  puts( "*** END OF TEST 13 ***" );
  rtems_test_exit( 0 );
}
