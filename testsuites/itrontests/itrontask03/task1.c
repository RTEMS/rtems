/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the task manager.
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

void Task_1()
{
  ER       status;

  /*
   * XXX - Change to itron routine.
   */

  puts( "TA1 - rtems_task_wake_after - sleep 1 second" );
  status = rtems_task_wake_after( 1*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  status = chg_pri( TA3_ID , 2 );
  directive_failed( status, "chg_pri" );

  puts( "TA1 - chg_pri - set TA3's priority to 2" );

  puts( "TA1 - sus_tsk - suspend TA2" );
  status = sus_tsk( TA2_ID  );
  directive_failed( status, "sus_tsk of TA2" );

  puts( "TA1 -  - delete TA2" );
  status = ter_tsk( TA2_ID );
  directive_failed( status, "ter_tsk of TA2" );

  /*
   * XXX - Change to itron routine.
   */

  puts( "TA1 - rtems_task_wake_after - sleep for 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "*** ITRON TASK TEST 3 ***" );
  rtems_test_exit( 0 );
}
