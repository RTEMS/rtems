/*  Screen7
 *
 *  This routine generates error screen 7 for test 9.
 *
 *  Input parameters:  NONE
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

#define MESSAGE_SIZE (sizeof(long) * 4)

void Screen7()
{
  long              buffer[ 4 ];
  rtems_unsigned32  size;
  rtems_unsigned32  count;
  rtems_status_code status;

  status = rtems_message_queue_broadcast( 100, buffer, MESSAGE_SIZE, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_broadcast with illegal id"
  );
  puts( "TA1 - rtems_message_queue_broadcast - RTEMS_INVALID_ID" );

  status = rtems_message_queue_create(
    0,
    3,
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

  /*
   *  The check for an object being global is only made if
   *  multiprocessing is enabled.
   */

#if defined(RTEMS_MULTIPROCESSING)
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    1,
    MESSAGE_SIZE,
    RTEMS_GLOBAL,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_MP_NOT_CONFIGURED,
    "rtems_message_queue_create of mp not configured"
  );
#endif
  puts( "TA1 - rtems_message_queue_create - Q 1 - RTEMS_MP_NOT_CONFIGURED" );

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

  status = rtems_message_queue_create(
    Queue_name[ 2 ],
    1,
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

  status = rtems_message_queue_delete( 0x10100 );
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

  status = rtems_message_queue_get_number_pending( 100, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_get_number_pending with illegal id"
  );
  puts( "TA1 - rtems_message_queue_get_number_pending - RTEMS_INVALID_ID" );

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
    "rtems_message_queue_receive wit illegal id"
  );
  puts( "TA1 - rtems_message_queue_receive - RTEMS_INVALID_ID" );

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
    3 * TICKS_PER_SECOND
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_message_queue_receive 3 second timeout"
  );

  puts(
    "TA1 - rtems_message_queue_receive - Q 1 - woke up with RTEMS_TIMEOUT"
  );

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
}
