/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
