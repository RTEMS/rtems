/*  Task_1
 *
 *  This task serves as a test task.  Verifies that a task can restart
 *  both itself and other tasks.
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
  rtems_task_argument   argument
)
{
  uint32_t    pass;
  rtems_status_code status;

  puts( "TA1 - is beginning to run" );

  pass = argument + 1;

  if ( pass == 5 ) {
    puts( "*** END OF TEST 6 ***" );
    rtems_test_exit( 0 );
  }

  if ( pass == 1 ) {
    puts( "TA1 - restarting TA3" );
    status = rtems_task_restart( Task_id[ 3 ], Restart_argument );
    directive_failed( status, "rtems_task_restart of TA3" );

    status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );
  }

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting TA2" );
  status = rtems_task_restart( Task_id[ 2 ], Restart_argument );
  directive_failed( status, "rtems_task_restart TA2" );

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting self" );
  status = rtems_task_restart( Task_id[ 1 ], pass );
  directive_failed( status, "rtems_task_restart of RTEMS_SELF" );
}
