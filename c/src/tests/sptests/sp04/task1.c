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

static void
showTaskSwitches (void)
{
  int i;
  int switches = taskSwitchLogIndex;

  for (i = 0 ; i < switches ; i++) {
      put_name( Task_name[taskSwitchLog[i].taskIndex], FALSE );
      print_time( "- ", &taskSwitchLog[i].when, "\n" );
  }
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_unsigned32  seconds;
  rtems_unsigned32  old_seconds;
  rtems_mode        previous_mode;
  rtems_time_of_day time;
  rtems_status_code status;
  rtems_unsigned32  start_time;
  rtems_unsigned32  end_time;

  puts( "TA1 - rtems_task_suspend - on Task 2" );
  status = rtems_task_suspend( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA1 - rtems_task_suspend - on Task 3" );
  status = rtems_task_suspend( Task_id[ 3 ] );
  directive_failed( status, "rtems_task_suspend of TA3" );

  status = rtems_clock_get( RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH, &start_time );
  directive_failed( status, "rtems_clock_get" );

  puts( "TA1 - killing time" );

  for ( ; ; ) {
    status = rtems_clock_get( RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH, &end_time );
    directive_failed( status, "rtems_clock_get" );

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

      status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
      directive_failed( status, "rtems_clock_get" );

      old_seconds = time.second;

      for ( seconds = 0 ; seconds < 6 ; ) {
        status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
        directive_failed( status, "rtems_clock_get" );

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
      puts( "*** END OF TEST 4 ***" );
      rtems_test_exit (0);
    }
  }
}
