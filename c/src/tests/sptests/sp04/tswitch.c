/*  Task_switch
 *
 *  This routine is the tswitch user extension.  It determines which
 *  task is being switched to and displays a message indicating the
 *  time and date that it gained control.
 *
 *  Input parameters:
 *    unused  - pointer to currently running TCB
 *    heir    - pointer to heir TCB
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

struct taskSwitchLog taskSwitchLog[1000];
int taskSwitchLogIndex;
volatile int testsFinished;

rtems_extension Task_switch( 
  rtems_tcb *unused,
  rtems_tcb *heir
)
{
  rtems_unsigned32  index;
  rtems_time_of_day time;
  rtems_status_code status;

  index = task_number( heir->Object.id ) - task_number( Task_id[1] ) + 1;

  switch( index ) {
    case 1:
    case 2:
    case 3:
      Run_count[ index ] += 1;

      status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
      directive_failed_with_level( status, "rtems_clock_get", 1 );

      if (taskSwitchLogIndex < (sizeof taskSwitchLog / sizeof taskSwitchLog[0])) {
        taskSwitchLog[taskSwitchLogIndex].taskIndex = index;
        taskSwitchLog[taskSwitchLogIndex].when = time;
        taskSwitchLogIndex++;
      }
      if ( time.second >= 16 )
	testsFinished = 1;
      break;

    case 0:
    default:
      break;
  }
}
