/*  Screen14
 *
 *  This routine generates error screen 14 for test 9.
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

void Screen14()
{
  rtems_status_code  status;
  rtems_time_of_day  time;

  status = rtems_timer_create( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_timer_create with illegal name"
  );
  puts( "TA1 - rtems_timer_create - RTEMS_INVALID_NAME" );

  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );
  puts( "TA1 - rtems_timer_create - 1 - RTEMS_SUCCESSFUL" );

  status = rtems_timer_create( 2, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_timer_create for too many"
  );
  puts( "TA1 - rtems_timer_create - 2 - RTEMS_TOO_MANY" );

  status = rtems_timer_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_delete with illegal id"
  );
  puts( "TA1 - rtems_timer_delete - local RTEMS_INVALID_ID" );

  status = rtems_timer_delete( 0x010100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_delete with illegal id"
  );
  puts( "TA1 - rtems_timer_delete - global RTEMS_INVALID_ID" );

  status = rtems_timer_ident( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_timer_ident with illegal name"
  );
  puts( "TA1 - rtems_timer_ident - RTEMS_INVALID_NAME" );

  status = rtems_timer_cancel( 0x010100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_cancel with illegal id"
  );
  puts( "TA1 - rtems_timer_cancel - RTEMS_INVALID_ID" );

  status = rtems_timer_reset( 0x010100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_reset with illegal id"
  );
  puts( "TA1 - rtems_timer_reset - RTEMS_INVALID_ID" );

  status = rtems_timer_reset( Timer_id[ 1 ] );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_timer_reset before initiated"
  );
  puts( "TA1 - rtems_timer_reset - RTEMS_NOT_DEFINED" );

  status = rtems_timer_fire_after(
    0x010100,
    5 * TICKS_PER_SECOND,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_ID" );

  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_fire_when( 0x010100, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_fire_when - RTEMS_INVALID_ID" );

  status = rtems_timer_fire_after( Timer_id[ 1 ], 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_NUMBER" );

  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  status = rtems_timer_fire_when( Timer_id[ 1 ], &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_fire_when with illegal time"
  );
  print_time(
    "TA1 - rtems_timer_fire_when - ",
    &time,
    " - RTEMS_INVALID_CLOCK\n"
  );

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get       - ", &time, "\n" );

  build_time( &time, 2, 5, 1990, 8, 30, 45, 0 );
  status = rtems_timer_fire_when( Timer_id[ 1 ], &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_fire_when before current time"
  );
  print_time(
    "TA1 - rtems_timer_fire_when - ",
    &time,
    " - before RTEMS_INVALID_CLOCK\n"
  );

/* timer server interface routines */

  status = rtems_timer_server_fire_after( 0, 5, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_after incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INCORRECT_STATE" );

  status = rtems_timer_server_fire_when( 0, &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_when incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INCORRECT_STATE" );

  status =
    rtems_timer_initiate_server( 0, 0, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_timer_initiate_server invalid priority"
  );
  puts( "TA1 - rtems_timer_initiate_server - RTEMS_INVALID_PRIORITY" );

  status =
    rtems_timer_initiate_server( RTEMS_TIMER_SERVER_DEFAULT_PRIORITY, 0, 0 );
  directive_failed( status, "rtems_timer_initiate_server" );
  puts( "TA1 - rtems_timer_initiate_server" );
  
  status = rtems_timer_server_fire_after(
    0x010100,
    5 * TICKS_PER_SECOND,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ID" );

  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_server_fire_when( 0x010100, &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INVALID_ID" );

  status = rtems_timer_server_fire_after( Timer_id[ 1 ], 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_server_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_NUMBER" );

  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when( Timer_id[ 1 ], &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_server_fire_when with illegal time"
  );
  print_time(
    "TA1 - rtems_timer_server_fire_when - ",
    &time,
    " - RTEMS_INVALID_CLOCK\n"
  );

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get       - ", &time, "\n" );

  build_time( &time, 2, 5, 1990, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when( Timer_id[ 1 ], &time, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_CLOCK,
    "rtems_timer_server_fire_when before current time"
  );
  print_time(
    "TA1 - rtems_timer_server_fire_when - ",
    &time,
    " - before RTEMS_INVALID_CLOCK\n"
  );
}
