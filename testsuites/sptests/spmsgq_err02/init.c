/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#define MESSAGE_SIZE (sizeof(long) * 4)

const char rtems_test_name[] = "SP MESSAGE QUEUE ERROR 2";

rtems_task Init(
  rtems_task_argument argument
)
{
  long              buffer[ 4 ];
  rtems_status_code status;
  
  TEST_BEGIN();
  Queue_name[ 1 ]      =  rtems_build_name( 'M', 'Q', '1', ' ' );
  Task_name[ 3 ]       =  rtems_build_name( 'T', 'A', '3', ' ' );
 
  status = rtems_task_create(
    Task_name[ 3 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );
  puts( "TA1 - rtems_task_create - TA3 created - RTEMS_SUCCESSFUL" );
  
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    2,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts( "TA1 - rtems_message_queue_create - Q 1 - 2 DEEP - RTEMS_SUCCESSFUL" );
   
    status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete successful" );
  puts( "TA1 - rtems_message_queue_delete - Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    2,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts(
    "TA1 - rtems_message_queue_create - Q 1 - 2 DEEP - RTEMS_SUCCESSFUL"
  );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send successful" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send successful" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_message_queue_send too many to limited queue"
  );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1 - RTEMS_TOO_MANY" );

  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete successful" );
  puts( "TA1 - rtems_message_queue_delete - Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    3,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts(
    "TA1 - rtems_message_queue_create - Q 1 - 3 DEEP - RTEMS_SUCCESSFUL"
  );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send successful" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send successful" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send successful" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_message_queue_send too many to limited queue"
  );
  puts(
    "TA1 - rtems_message_queue_send - BUFFER 4 TO Q 1 - RTEMS_TOO_MANY"
  );

  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete successful" );
  puts( "TA1 - rtems_message_queue_delete - Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    3,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts(
    "TA1 - rtems_message_queue_create - Q 1 - 3 DEEP - RTEMS_SUCCESSFUL"
  );

  puts( "TA1 - rtems_task_start - start TA3 - RTEMS_SUCCESSFUL" );
  status = rtems_task_start( Task_id[ 3 ], Task_3, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  puts( "TA1 - rtems_message_queue_delete - delete Q 1 - RTEMS_SUCCESSFUL" );
  status = rtems_message_queue_delete( Queue_id[ 1 ] );
  directive_failed( status, "rtems_message_queue_delete successful" );

  puts( "TA1 - rtems_task_wake_after - yield processor - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after (yield)" );

  TEST_END();
}
