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
const char rtems_test_name[] = "SP MESSAGE QUEUE ERROR 1";

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  long              buffer[ 4 ];
  uint32_t          count;
  size_t            size;
  rtems_status_code status;

  rtems_test_assert( MAX_MESSAGES == 2 );
  
  TEST_BEGIN();
  Queue_name[ 1 ]      =  rtems_build_name( 'M', 'Q', '1', ' ' );
  Queue_name[ 2 ]      =  rtems_build_name( 'M', 'Q', '2', ' ' );
  
  status = rtems_message_queue_broadcast( 100, buffer, MESSAGE_SIZE, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_broadcast with illegal id"
  );
  puts( "TA1 - rtems_message_queue_broadcast - RTEMS_INVALID_ID" );

  /* null ID parameter */
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    MAX_MESSAGES,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_create with null param"
  );
  puts( "TA1 - rtems_message_queue_create - NULL Id - RTEMS_INVALID_ADDRESS" );

  /* count == 0 */
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    0,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_message_queue_create with 0 count"
  );
  puts( "TA1 - rtems_message_queue_create - count = 0 - RTEMS_INVALID_NUMBER" );

  /* max size == 0 */
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    MAX_MESSAGES,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_message_queue_create with 0 msg size"
  );
  puts( "TA1 - rtems_message_queue_create - size = 0 - RTEMS_INVALID_SIZE" );

  /* bad name parameter */
  status = rtems_message_queue_create(
    0,
    MAX_MESSAGES,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_message_queue_create with illegal name"
  );
  puts( "TA1 - rtems_message_queue_create - Q 1 - RTEMS_INVALID_NAME" );

  /* not enough memory for messages */
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    UINT32_MAX / ( sizeof( uintptr_t ) + sizeof( CORE_message_queue_Buffer ) ),
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_message_queue_create unsatisfied"
  );
  puts( "TA1 - rtems_message_queue_create - Q 2 - RTEMS_UNSATISFIED" );

  /* too large a request for messages */
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    UINT32_MAX,
    SIZE_MAX - sizeof( uintptr_t ) + 1 - sizeof( CORE_message_queue_Buffer ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_message_queue_create invalid number"
  );
  puts( "TA1 - rtems_message_queue_create - Q 2 - RTEMS_INVALID_NUMBER" );

  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    MAX_MESSAGES,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts( "TA1 - rtems_message_queue_create - Q 1 - 2 DEEP - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_create(
    Queue_name[ 2 ],
    MAX_MESSAGES,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_message_queue_create of too many"
  );
  puts( "TA1 - rtems_message_queue_create - Q 2 - RTEMS_TOO_MANY" );

  status = rtems_message_queue_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_create with illegal id"
  );
  puts( "TA1 - rtems_message_queue_delete - unknown RTEMS_INVALID_ID" );

  status = rtems_message_queue_delete( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_delete with local illegal id"
  );
  puts( "TA1 - rtems_message_queue_delete - local RTEMS_INVALID_ID" );

  status = rtems_message_queue_ident( 100, RTEMS_SEARCH_ALL_NODES, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_message_queue_ident with illegal name"
  );
  puts( "TA1 - rtems_message_queue_ident - RTEMS_INVALID_NAME" );

  /* number pending - bad Id */
  status = rtems_message_queue_get_number_pending( Queue_id[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_get_number_pending with NULL param"
  );
  puts("TA1 - rtems_message_queue_get_number_pending - RTEMS_INVALID_ADDRESS");

  /* number pending - bad Id */
  status = rtems_message_queue_get_number_pending( 100, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_get_number_pending with illegal id"
  );
  puts( "TA1 - rtems_message_queue_get_number_pending - RTEMS_INVALID_ID" );

  /* flush null param */
  status = rtems_message_queue_flush( Queue_id[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_flush with NULL param"
  );
  puts( "TA1 - rtems_message_queue_flush - RTEMS_INVALID_ADDRESS" );

  /* flush invalid id */
  status = rtems_message_queue_flush( 100, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_flush with illegal id"
  );
  puts( "TA1 - rtems_message_queue_flush - RTEMS_INVALID_ID" );

  status = rtems_message_queue_receive(
    100,
    (long (*)[4]) buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    0
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_receive with illegal id"
  );
  puts( "TA1 - rtems_message_queue_receive - RTEMS_INVALID_ID" );

  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    NULL,
    &size,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_receive NULL buffer"
  );
  puts(
    "TA1 - rtems_message_queue_receive - Q 1 - "
      "RTEMS_INVALID_ADDRESS NULL buffer"
  );

  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4]) buffer,
    NULL,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_receive NULL size"
  );
  puts(
    "TA1 - rtems_message_queue_receive - Q 1 - "
      "RTEMS_INVALID_ADDRESS NULL size"
  );

  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4]) buffer,
    &size,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_message_queue_receive unsatisfied"
  );
  puts( "TA1 - rtems_message_queue_receive - Q 1 - RTEMS_UNSATISFIED" );

  puts( "TA1 - rtems_message_queue_receive - Q 1 - timeout in 3 seconds" );
  status = rtems_message_queue_receive(
    Queue_id[ 1 ],
    (long (*)[4]) buffer,
    &size,
    RTEMS_DEFAULT_OPTIONS,
    3 * rtems_clock_get_ticks_per_second()
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_message_queue_receive 3 second timeout"
  );

  puts(
    "TA1 - rtems_message_queue_receive - Q 1 - woke up with RTEMS_TIMEOUT"
  );

  /* send NULL message*/
  status = rtems_message_queue_send( Queue_id[ 1 ], NULL, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_send with NULL buffer"
  );
  puts(
    "TA1 - rtems_message_queue_send - NULL buffer - RTEMS_INVALID_ADDRESS"
  );

  /* send bad id */
  status = rtems_message_queue_send( 100, buffer, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_send with illegal id"
  );
  puts( "TA1 - rtems_message_queue_send - RTEMS_INVALID_ID" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  directive_failed( status, "rtems_message_queue_send" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], buffer, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_message_queue_send too many to a limited queue"
  );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1 - RTEMS_TOO_MANY" );

  /* urgent NULL message*/
  status = rtems_message_queue_urgent( Queue_id[ 1 ], NULL, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_urgent with NULL buffer"
  );
  puts(
    "TA1 - rtems_message_queue_urgent - NULL buffer - RTEMS_INVALID_ADDRESS"
  );

  /* urgent bad Id */
  status = rtems_message_queue_urgent( 100, buffer, MESSAGE_SIZE );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_urgent with illegal id"
  );
  puts( "TA1 - rtems_message_queue_urgent - RTEMS_INVALID_ID" );

  status = rtems_message_queue_broadcast(
     Queue_id[ 1 ], NULL, MESSAGE_SIZE, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_broadcast with NULL count"
  );
  puts(
    "TA1 - rtems_message_queue_broadcast - NULL buffer - RTEMS_INVALID_ADDRESS"
  );

  status = rtems_message_queue_broadcast(
     Queue_id[ 1 ], buffer, MESSAGE_SIZE + 1, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_SIZE,
    "rtems_message_queue_broadcast with too large"
  );
  puts(
    "TA1 - rtems_message_queue_broadcast - too large - RTEMS_INVALID_SIZE"
  );

  status = rtems_message_queue_broadcast(
      Queue_id[ 1 ], buffer, MESSAGE_SIZE, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_message_queue_broadcast with NULL count"
  );
  puts(
    "TA1 - rtems_message_queue_broadcast - NULL count - RTEMS_INVALID_ADDRESS"
  );

  TEST_END();
}
