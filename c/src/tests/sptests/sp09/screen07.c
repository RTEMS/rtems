/*  Screen7
 *
 *  This routine generates error screen 7 for test 9.
 *
 *  Input parameters:  NONE
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

void Screen7()
{
  long              buffer[ 4 ];
  rtems_unsigned32  count;
  rtems_status_code status;

  status = rtems_message_queue_broadcast( 100, (long (*)[4]) buffer, &count );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_broadcast with illegal id"
  );
  puts( "TA1 - rtems_message_queue_broadcast - RTEMS_INVALID_ID" );

  status = rtems_message_queue_create(
    0,
    3,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_message_queue_create with illegal name"
  );

  puts( "TA1 - rtems_message_queue_create - Q 1 - RTEMS_INVALID_NAME" );
  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    1,
    RTEMS_GLOBAL,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_MP_NOT_CONFIGURED,
    "rtems_message_queue_create of mp not configured"
  );
  puts( "TA1 - rtems_message_queue_create - Q 1 - RTEMS_MP_NOT_CONFIGURED" );

  status = rtems_message_queue_create(
    Queue_name[ 1 ],
    2,
    RTEMS_LIMIT,
    &Queue_id[ 1 ]
  );
  directive_failed( status, "rtems_message_queue_create successful" );
  puts(
    "TA1 - rtems_message_queue_create - Q 1 - RTEMS_LIMIT - RTEMS_SUCCESSFUL"
  );

  status = rtems_message_queue_create(
    Queue_name[ 2 ],
    1,
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

  status = rtems_message_queue_send( 100, (long (*)[4]) buffer );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_message_queue_send with illegal id"
  );
  puts( "TA1 - rtems_message_queue_send - RTEMS_INVALID_ID" );

  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4]) buffer );
  directive_failed( status, "rtems_message_queue_send" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 1 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4]) buffer );
  directive_failed( status, "rtems_message_queue_send" );
  puts( "TA1 - rtems_message_queue_send - BUFFER 2 TO Q 1 - RTEMS_SUCCESSFUL" );

  status = rtems_message_queue_send( Queue_id[ 1 ], (long (*)[4]) buffer );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_message_queue_send too many to a limited queue"
  );
  puts( "TA1 - rtems_message_queue_send - BUFFER 3 TO Q 1 - RTEMS_TOO_MANY" );
}
