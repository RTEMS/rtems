/*  Task_2
 *
 *  This routine serves as a test task.  It verifies that one task can
 *  send signals to another task ( invoking the other task's RTEMS_ASR ).
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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "TA2 - rtems_signal_send - RTEMS_SIGNAL_17 to TA1" );
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_17 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA2 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  puts("TA2 - rtems_signal_send - RTEMS_SIGNAL_18 and RTEMS_SIGNAL_19 to TA1");
  status = rtems_signal_send( Task_id[ 1 ], RTEMS_SIGNAL_18 | RTEMS_SIGNAL_19 );
  directive_failed( status, "rtems_signal_send" );

  puts( "TA2 - rtems_task_wake_after - yield processor" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "*** END OF TEST 14 ***" );
  rtems_test_exit( 0 );
}
