/*  Screen10
 *
 *  This routine generates error screen 10 for test 9.
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

void Screen10()
{
  rtems_status_code status;

  status = rtems_rate_monotonic_create( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_rate_monotonic_create with illegal name"
  );
  puts( "TA1 - rtems_rate_monotonic_create - RTEMS_INVALID_NAME" );

  status = rtems_rate_monotonic_create( Period_name[ 1 ], &Period_id[ 1 ] );
  directive_failed( status, "rtems_rate_monotonic_create successful" );
  puts( "TA1 - rtems_rate_monotonic_create - RTEMS_SUCCESSFUL" );

  status = rtems_rate_monotonic_create( Period_name[ 1 ], &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_rate_monotonic_create of too many"
  );
  puts( "TA1 - rtems_rate_monotonic_create - RTEMS_TOO_MANY" );

  status = rtems_rate_monotonic_ident( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_rate_monotonic_ident with illegal name"
  );
  puts( "TA1 - rtems_rate_monotonic_ident - RTEMS_INVALID_NAME" );

  status = rtems_rate_monotonic_period( 100, 5 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_period with illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_period - unknown RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_period( 0x10100, 5 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_period with illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_period - local RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], RTEMS_PERIOD_STATUS );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_rate_monotonic_period status not defined"
  );
  puts(
    "TA1 - rtems_rate_monotonic_period(RTEMS_PERIOD_STATUS) - RTEMS_NOT_DEFINED"
  );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], 100 );
  directive_failed( status, "rtems_rate_monotonic_period successful" );
  puts( "TA1 - rtems_rate_monotonic_period - 100 ticks - RTEMS_SUCCESSFUL" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], RTEMS_PERIOD_STATUS );
  directive_failed( status, "rtems_rate_monotonic_period status" );
  puts(
    "TA1 - rtems_rate_monotonic_period(RTEMS_PERIOD_STATUS) - RTEMS_SUCCESSFUL"
  );

  while ( FOREVER ) {

     status = rtems_rate_monotonic_period(Period_id[ 1 ], RTEMS_PERIOD_STATUS);

     if ( status == RTEMS_TIMEOUT ) break;

     directive_failed(
       status,
       "rtems_rate_monotonic_period waiting for timeout"
     );
  }
  puts(
    "TA1 - rtems_rate_monotonic_period(RTEMS_PERIOD_STATUS) - RTEMS_TIMEOUT"
  );

  status = rtems_rate_monotonic_cancel( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_cancel with illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_cancel - unknown RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_cancel( 0x10100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_cancel will illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_cancel - local RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_cancel( Period_id[ 1 ] );
  directive_failed( status, "rtems_rate_monotonic_cancel" );
  puts( "TA1 - rtems_rate_monotonic_cancel - RTEMS_SUCCESSFUL" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], 5 );
  directive_failed( status, "rtems_rate_monotonic_period restart" );

  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_rate_monotonic_period( Period_id[ 1 ], 5 );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_rate_monotonic_period"
  );
  puts( "TA1 - rtems_rate_monotonic_period - 5 ticks - RTEMS_TIMEOUT" );

  status = rtems_task_start( Task_id[ 4 ], Task_4, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  puts( "TA1 - yielding to TA4" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  status = rtems_rate_monotonic_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_delete with illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_delete - unknown RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_delete( 0x10100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_rate_monotonic_delete with illegal id"
  );
  puts( "TA1 - rtems_rate_monotonic_delete - local RTEMS_INVALID_ID" );

  status = rtems_rate_monotonic_delete( Period_id[ 1 ] );
  directive_failed( status, "rtems_rate_monotonic_delete" );
  puts( "TA1 - rtems_rate_monotonic_delete - RTEMS_SUCCESSFUL" );
}
