/*  Task_1
 *
 *  This routine serves as a test task.  It verifies that tasks can
 *  be suspended and resumed.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  NOTE: The rtems_task_suspend() directives fail on the first iteration.
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
  rtems_id          tid2;
  rtems_id          tid3;
  uint32_t    pass;
  rtems_status_code status;

  status = rtems_task_ident( Task_name[ 2 ], 1, &tid2 );
  directive_failed( status, "rtems_task_ident of TA2" );

  status = rtems_task_ident( Task_name[ 3 ], 1, &tid3 );
  directive_failed( status, "rtems_task_ident of TA3" );

  for ( pass=1 ; pass <= 3 ; pass++ ) {

    puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after of TA1" );

    puts( "TA1 - rtems_task_suspend - suspend TA3" );
    status = rtems_task_suspend( tid3 );
    if ( pass == 1 ) {
      fatal_directive_status(
         status,
         RTEMS_ALREADY_SUSPENDED,
         "rtems_task_suspend of TA3"
      );
    } else {
      directive_failed( status, "rtems_task_suspend of TA3" );
    }

    puts( "TA1 - rtems_task_resume - resume TA2" );
    status = rtems_task_resume( tid2 );
    directive_failed( status, "rtems_task_resume of TA2" );

    puts( "TA1 - rtems_task_wake_after - sleep 5 seconds" );
    status = rtems_task_wake_after( 5*rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "TA1 - rtems_task_suspend - suspend TA2" );
    status = rtems_task_suspend( tid2 );
    directive_failed( status, "rtems_task_suspend of TA2" );

    puts( "TA1 - rtems_task_resume - resume TA3" );
    status = rtems_task_resume( tid3 );
    directive_failed( status, "rtems_task_resume" );
  }

  puts( "*** END OF TEST 5 ***" );
  rtems_test_exit( 0 );
}
