/*  Task_3
 *
 *  This routine serves as a test task.  It simply sleeps for 5 seconds
 *  and then deletes itself.
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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

void Task_3()
{
  rtems_status_code status;
  int               i;

  for(i=0; i<5; i++) {
    puts("TA3 - frsm_tsk TA1");
    status = frsm_tsk( TA1_ID );
    directive_failed( status, "TA3 - frsm_tsk TA1" );
    
    puts("TA3 - rsm_tsk TA2");
    status = rsm_tsk( TA2_ID );
    directive_failed( status, "TA3 - rsm_tsk TA2" );

    puts( "TA3 - rot_rdq priority 2 queue");
    status = rot_rdq( 2 );
    directive_failed( status, "rot_rdq for 2" );

    /*
     * XXX - Convert Later.
     */

    puts( "TA3 - rtems_task_wake_after - sleep 1 second" );
    status = rtems_task_wake_after( TICKS_PER_SECOND );
    directive_failed( status, "rtems_task_wake_after in TA3" );
  }

  puts( "*** ITRON TASK TEST 4 ***" );
  rtems_test_exit( 0 );
}

