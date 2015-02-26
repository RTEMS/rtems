/*  Task_1
 *
 *  This test serves as a test task.  It verifies timeslicing activities
 *  and tswitch extension processing.
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

static void
showTaskSwitches (void)
{
  unsigned int i;
  unsigned int switches = taskSwitchLogIndex;

  for (i = 0 ; i < switches ; i++) {
      put_name( Task_name[taskSwitchLog[i].taskIndex], FALSE );
      print_time( "- ", &taskSwitchLog[i].when, "\n" );
  }
}

static int test_no_preempt_step;

static rtems_id high_task_id;

static rtems_id low_task_id;

static void high_task( rtems_task_argument arg )
{
  rtems_status_code sc;

  rtems_test_assert( test_no_preempt_step == 2 );
  test_no_preempt_step = 3;

  sc = rtems_event_transient_send( Task_id[ 1 ] );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void low_task( rtems_task_argument arg )
{
  rtems_test_assert( test_no_preempt_step == 1 );
  test_no_preempt_step = 2;

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void no_preempt_timer( rtems_id id, void *arg )
{
  rtems_status_code sc;

  rtems_test_assert( test_no_preempt_step == 0 );
  test_no_preempt_step = 1;

  sc = rtems_task_start( low_task_id, low_task, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_start( high_task_id, high_task, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

static void test_no_preempt( void )
{
  rtems_status_code sc;
  rtems_id id;

  rtems_test_assert( test_no_preempt_step == 0 );

  sc = rtems_task_delete( Task_id[ 2 ] );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_delete( Task_id[ 3 ] );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &high_task_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &low_task_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_timer_create( rtems_build_name( 'N', 'O', 'P', 'R' ), &id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_timer_fire_after( id, 1, no_preempt_timer, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_timer_delete( id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_delete( high_task_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_delete( low_task_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_test_assert( test_no_preempt_step == 3 );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  uint32_t    seconds;
  uint32_t    old_seconds;
  rtems_mode        previous_mode;
  rtems_time_of_day time;
  rtems_status_code status;
  uint32_t    start_time;
  uint32_t    end_time;

  puts( "TA1 - rtems_task_suspend - on Task 2" );
  status = rtems_task_suspend( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA1 - rtems_task_suspend - on Task 3" );
  status = rtems_task_suspend( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_suspend of TA3" );

  status = rtems_clock_get_seconds_since_epoch( &start_time );
  directive_failed( status, "rtems_clock_get_seconds_since_epoch" );

  puts( "TA1 - killing time" );

  for ( ; ; ) {
    status = rtems_clock_get_seconds_since_epoch( &end_time );
    directive_failed( status, "rtems_clock_get_seconds_since_epoch" );

    if ( end_time > (start_time + 2) )
      break;
  }

  puts( "TA1 - rtems_task_resume - on Task 2" );
  status = rtems_task_resume( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_resume of TA2" );

  puts( "TA1 - rtems_task_resume - on Task 3" );
  status = rtems_task_resume( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_resume of TA3" );

  while ( FOREVER ) {
    if ( Run_count[ 1 ] >= 3 ) {
      puts( "TA1 - rtems_task_mode - change mode to NO RTEMS_PREEMPT" );

      status = rtems_task_mode(
        RTEMS_NO_PREEMPT,
        RTEMS_PREEMPT_MASK,
        &previous_mode
      );
      directive_failed( status, "rtems_task_mode" );

      status = rtems_clock_get_tod( &time );
      directive_failed( status, "rtems_clock_get_tod" );

      old_seconds = time.second;

      for ( seconds = 0 ; seconds < 6 ; ) {
        status = rtems_clock_get_tod( &time );
        directive_failed( status, "rtems_clock_get_tod" );

        if ( time.second != old_seconds ) {
          old_seconds = time.second;
          seconds++;
          print_time( "TA1 - ", &time, "\n" );
        }
      }

      puts( "TA1 - rtems_task_mode - change mode to RTEMS_PREEMPT" );
      status = rtems_task_mode(
        RTEMS_PREEMPT,
        RTEMS_PREEMPT_MASK,
        &previous_mode
      );
      directive_failed( status, "rtems_task_mode" );

      while ( !testsFinished );
      showTaskSwitches ();

      puts( "TA1 - rtems_extension_delete - successful" );
      status = rtems_extension_delete( Extension_id[1] );
      directive_failed( status, "rtems_extension_delete" );

      test_no_preempt();

      TEST_END();
      rtems_test_exit (0);
    }
  }
}
