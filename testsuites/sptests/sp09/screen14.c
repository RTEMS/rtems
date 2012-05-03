/*  Screen14
 *
 *  This routine generates error screen 14 for test 9.
 *
 *  Input parameters:  NONE
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

void Screen14()
{
  rtems_status_code       status;
  rtems_time_of_day       time;
  rtems_timer_information timer_info;
  bool                    skipUnsatisfied;

  /* NULL Id */
  status = rtems_timer_create( Timer_name[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_create NULL param"
  );
  puts( "TA1 - rtems_timer_create - RTEMS_INVALID_ADDRESS" );

  /* bad name */
  status = rtems_timer_create( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_timer_create with illegal name"
  );
  puts( "TA1 - rtems_timer_create - RTEMS_INVALID_NAME" );

  /* OK */
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

  status = rtems_timer_delete( rtems_build_id( 1, 1, 1, 256 ) );
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

  status = rtems_timer_cancel( rtems_build_id( 1, 1, 1, 256 ) );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_cancel with illegal id"
  );
  puts( "TA1 - rtems_timer_cancel - RTEMS_INVALID_ID" );

  status = rtems_timer_reset( rtems_build_id( 1, 1, 1, 256 ) );
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

  /* bad id */
  status = rtems_timer_fire_after(
    rtems_build_id( 1, 1, 1, 256 ),
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_ID" );

  /* bad id */
  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_fire_when(
    rtems_build_id( 1, 1, 1, 256 ),
    &time,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_fire_when - RTEMS_INVALID_ID" );

  /* NULL routine */
  status = rtems_timer_fire_after( Timer_id[ 1 ], 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_fire_after with NULL handler"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_ADDRESS" );

  /* 0 ticks */
  status = rtems_timer_fire_after( Timer_id[ 1 ], 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_fire_after - RTEMS_INVALID_NUMBER" );

  /* NULL routine */
  status = rtems_timer_fire_when( Timer_id[ 1 ], &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_fire_when with NULL handler"
  );
  puts( "TA1 - rtems_timer_fire_when - RTEMS_INVALID_ADDRESS" );

  /* invalid time -- before RTEMS epoch */
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

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod       - ", &time, "\n" );

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

  /* null param */
  status = rtems_timer_get_information( Timer_id[ 1 ], NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_get_information with NULL param"
  );
  puts( "TA1 - rtems_timer_get_information - RTEMS_INVALID_ADDRESS" );

  /* invalid id */
  status = rtems_timer_get_information( 100, &timer_info );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_get_information with illegal id"
  );
  puts( "TA1 - rtems_timer_get_information - RTEMS_INVALID_ID" );

/* timer server interface routines */

  /* incorrect state */
  status = rtems_timer_server_fire_after( 0, 5, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_after incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INCORRECT_STATE" );

  /* incorrect state */
  status = rtems_timer_server_fire_when( 0, &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INCORRECT_STATE,
    "rtems_timer_server_fire_when incorrect state"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INCORRECT_STATE" );

  /* invalid priority */
  status = rtems_timer_initiate_server( 0, 0, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_PRIORITY,
    "rtems_timer_initiate_server invalid priority"
  );
  puts( "TA1 - rtems_timer_initiate_server - RTEMS_INVALID_PRIORITY" );

  skipUnsatisfied = false;
  #if defined(__m32c__)
    skipUnsatisfied = true;
  #endif
  if (skipUnsatisfied) {
    puts( "TA1 - rtems_timer_initiate_server - RTEMS_UNSATISFIED -- SKIPPED" );
  } else {
    status = rtems_timer_initiate_server(
      RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
      0x10000000,
      0
    );
    fatal_directive_status(
      status,
      RTEMS_UNSATISFIED,
      "rtems_timer_initiate_server too much stack "
    );
    puts( "TA1 - rtems_timer_initiate_server - RTEMS_UNSATISFIED" );
  }

  status =
    rtems_timer_initiate_server( RTEMS_TIMER_SERVER_DEFAULT_PRIORITY, 0, 0 );
  directive_failed( status, "rtems_timer_initiate_server" );
  puts( "TA1 - rtems_timer_initiate_server - SUCCESSFUL" );

  /* NULL routine */
  status = rtems_timer_server_fire_after( Timer_id[ 1 ], 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_after NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ADDRESS" );

  /* bad Id */
  status = rtems_timer_server_fire_after(
    rtems_build_id( 1, 1, 1, 256 ),
    5 * rtems_clock_get_ticks_per_second(),
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_after illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ID" );

  /* bad id */
  build_time( &time, 12, 31, 1994, 9, 0, 0, 0 );
  status = rtems_timer_server_fire_when(
    rtems_build_id( 1, 1, 1, 256 ),
    &time,
    Delayed_routine,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_timer_server_fire_when with illegal id"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INVALID_ID" );

  /* NULL routine */
  status = rtems_timer_server_fire_after( Timer_id[ 1 ], 1, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_after NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_ADDRESS" );

  /* 0 ticks */
  status = rtems_timer_server_fire_after(
    Timer_id[ 1 ], 0, Delayed_routine, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_timer_server_fire_after with 0 ticks"
  );
  puts( "TA1 - rtems_timer_server_fire_after - RTEMS_INVALID_NUMBER" );

  /* illegal time */
  build_time( &time, 2, 5, 1987, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when(
    Timer_id[ 1 ], &time, Delayed_routine, NULL );
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

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod       - ", &time, "\n" );

  /* when NULL routine */
  status = rtems_timer_server_fire_when( Timer_id[ 1 ], &time, NULL, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_timer_server_fire_when NULL routine"
  );
  puts( "TA1 - rtems_timer_server_fire_when - RTEMS_INVALID_ADDRESS" );

  /* before current time */
  build_time( &time, 2, 5, 1990, 8, 30, 45, 0 );
  status = rtems_timer_server_fire_when(
    Timer_id[ 1 ], &time, Delayed_routine, NULL );
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
