/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

volatile int TSR_fired;

rtems_timer_service_routine Should_not_fire_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  TSR_fired = 1;
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id                 tmid;
  rtems_id                 tmid2;
  rtems_time_of_day        time;
  rtems_status_code        status;
  rtems_timer_information  info;

/* Get id */

  puts( "TA1 - rtems_timer_ident - identing timer 1" );
  status = rtems_timer_ident( Timer_name[ 1 ], &tmid );
  directive_failed( status, "rtems_timer_ident" );
  printf( "TA1 - timer 1 has id (0x%x)\n", tmid );

  puts( "TA1 - rtems_timer_ident - identing timer 2" );
  status = rtems_timer_ident( Timer_name[ 2 ], &tmid2 );
  directive_failed( status, "rtems_timer_ident" );
  printf( "TA1 - timer 2 has id (0x%x)\n", tmid2 );

/* make sure insertion does not unintentionally fire a timer per PR147 */

  TSR_fired = 0;

  puts( "TA1 - rtems_timer_server_fire_after - 1 second" );
  status = rtems_timer_server_fire_after(
    tmid, TICKS_PER_SECOND, Should_not_fire_TSR, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 1/2 second" );
  status = rtems_task_wake_after( TICKS_PER_SECOND / 2 );
  directive_failed( status, "rtems_timer_server_fire_after" );

  directive_failed( status, "rtems_timer_server_fire_after" );
  puts( "TA1 - rtems_timer_server_fire_after - timer 2 in 1/2 second" );
  status = rtems_timer_server_fire_after(
    tmid2, TICKS_PER_SECOND / 2, Should_not_fire_TSR, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  if ( TSR_fired ) {
    puts( "TA1 - TSR fired and should not have!" );
    rtems_test_exit(1); 
  }

  puts( "TA1 - rtems_timer_cancel - timer 1" );
  status = rtems_timer_cancel( tmid );
  directive_failed( status, "rtems_timer_cancel" );

  puts( "TA1 - rtems_timer_cancel - timer 2" );
  status = rtems_timer_cancel( tmid2 );
  directive_failed( status, "rtems_timer_cancel" );

  
/* now check that rescheduling an active timer works OK. */
  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 30 seconds" );
  status = rtems_timer_server_fire_after(
    tmid, 30 * TICKS_PER_SECOND, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_timer_server_fire_after - timer 2 in 60 seconds" );
  status = rtems_timer_server_fire_after(
    tmid2, 60 * TICKS_PER_SECOND, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  status = rtems_timer_get_information( tmid, &info );
  printf( "Timer 1 scheduled for %d ticks since boot\n",
    info.start_time + info.initial );
  printf( "Timer Server scheduled for %d ticks since boot\n",
    _Timer_Server->Timer.initial + _Timer_Server->Timer.start_time );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  directive_failed( status, "rtems_timer_wake_after" );

  puts( "TA1 - rtems_timer_server_fire_after - timer 2 in 60 seconds" );
  status = rtems_timer_server_fire_after(
    tmid2, 60 * TICKS_PER_SECOND, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  status = rtems_timer_get_information( tmid, &info );
  directive_failed( status, "rtems_timer_get_information" );
  printf( "Timer 1 scheduled for %d ticks since boot\n",
    info.start_time + info.initial );
  printf( "Timer Server scheduled for %d ticks since boot\n",
    _Timer_Server->Timer.initial + _Timer_Server->Timer.start_time );
  assert(  (info.start_time + info.initial) == 
    (_Timer_Server->Timer.initial + _Timer_Server->Timer.start_time) );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  directive_failed( status, "rtems_timer_wake_after" );

  puts( "TA1 - rtems_timer_server_fire_after - timer 2 in 60 seconds" );
  status = rtems_timer_server_fire_after(
    tmid2, 60 * TICKS_PER_SECOND, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_after" );

  status = rtems_timer_get_information( tmid, &info );
  directive_failed( status, "rtems_timer_get_information" );
  printf( "Timer 1 scheduled for %d ticks since boot\n",
    info.start_time + info.initial );
  printf( "Timer Server scheduled for %d ticks since boot\n",
    _Timer_Server->Timer.initial + _Timer_Server->Timer.start_time );
  assert(  (info.start_time + info.initial) == 
    (_Timer_Server->Timer.initial + _Timer_Server->Timer.start_time) );

  puts( "TA1 - rtems_timer_cancel - timer 1" );
  status = rtems_timer_cancel( tmid );
  directive_failed( status, "rtems_timer_cancel" );

  puts( "TA1 - rtems_timer_cancel - timer 2" );
  status = rtems_timer_cancel( tmid2 );
  directive_failed( status, "rtems_timer_cancel" );

/* after which is allowed to fire */

  Print_time();

  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_server_fire_after(
    tmid,
    3 * TICKS_PER_SECOND,
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

/* after which is reset and allowed to fire */

  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_server_fire_after(
    tmid,
    3 * TICKS_PER_SECOND,
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  Print_time();

  puts( "TA1 - rtems_timer_reset - timer 1" );
  status = rtems_timer_reset( tmid );
  directive_failed( status, "rtems_timer_reset" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

  rtems_test_pause();

  /*
   *  Reset the time since we do not know how long the user waited
   *  before pressing <cr> at the pause.  This insures that the 
   *  actual output matches the screen.
   */

  build_time( &time, 12, 31, 1988, 9, 0, 7, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

/* after which is canceled */

  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_server_fire_after(
    tmid,
    3 * TICKS_PER_SECOND,
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_timer_cancel - timer 1" );
  status = rtems_timer_cancel( tmid );
  directive_failed( status, "rtems_timer_cancel" );

/* when which is allowed to fire */

  Print_time();

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( status, "rtems_clock_get" );

  time.second += 3;

  puts( "TA1 - rtems_timer_server_fire_when - timer 1 in 3 seconds" );
  status = rtems_timer_server_fire_when( tmid, &time, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_when" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

/* when which is canceled */

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  directive_failed( status, "rtems_clock_get" );

  time.second += 3;

  puts( "TA1 - rtems_timer_server_fire_when - timer 1 in 3 seconds" );
  status = rtems_timer_server_fire_when( tmid, &time, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_server_fire_when" );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( 1 * TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  Print_time();

  puts( "TA1 - rtems_timer_cancel - timer 1" );
  status = rtems_timer_cancel( tmid );
  directive_failed( status, "rtems_timer_cancel" );

/* delete */
  puts( "TA1 - rtems_task_wake_after - YIELD (only task at priority)" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - timer_deleting - timer 1" );
  status = rtems_timer_delete( tmid );
  directive_failed( status, "rtems_timer_delete" );

  puts( "*** END OF TEST 31 *** " );
  rtems_test_exit( 0 );
}
