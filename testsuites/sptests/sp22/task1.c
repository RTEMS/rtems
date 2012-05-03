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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          tmid;
  rtems_time_of_day time;
  rtems_status_code status;

/* Get id */

  puts( "TA1 - rtems_timer_ident - identing timer 1" );
  status = rtems_timer_ident( Timer_name[ 1 ], &tmid );
  directive_failed( status, "rtems_timer_ident" );
  printf( "TA1 - timer 1 has id (0x%" PRIxrtems_id ")\n", tmid );

/* after which is allowed to fire */

  Print_time();

  puts( "TA1 - rtems_timer_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_fire_after(
    tmid,
    3 * rtems_clock_get_ticks_per_second(),
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

/* after which is reset and allowed to fire */

  puts( "TA1 - rtems_timer_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_fire_after(
    tmid,
    3 * rtems_clock_get_ticks_per_second(),
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
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

  puts( "TA1 - rtems_timer_fire_after - timer 1 in 3 seconds" );
  status = rtems_timer_fire_after(
    tmid,
    3 * rtems_clock_get_ticks_per_second(),
    Delayed_resume,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  puts( "TA1 - rtems_timer_cancel - timer 1" );
  status = rtems_timer_cancel( tmid );
  directive_failed( status, "rtems_timer_cancel" );

/* when which is allowed to fire */

  Print_time();

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );

  time.second += 3;

  puts( "TA1 - rtems_timer_fire_when - timer 1 in 3 seconds" );
  status = rtems_timer_fire_when( tmid, &time, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_fire_when" );

  puts( "TA1 - rtems_task_suspend( RTEMS_SELF )" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  Print_time();

/* when which is canceled */

  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );

  time.second += 3;

  puts( "TA1 - rtems_timer_fire_when - timer 1 in 3 seconds" );
  status = rtems_timer_fire_when( tmid, &time, Delayed_resume, NULL );
  directive_failed( status, "rtems_timer_fire_when" );

  puts( "TA1 - rtems_task_wake_after - 1 second" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
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

  puts( "*** END OF TEST 22 *** " );
  rtems_test_exit( 0 );
}
