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

/*
 *  Only require 8 task switches on sis/gdb.  --joel 2 Feb 2011
 */
struct taskSwitchLog taskSwitchLog[25];
int taskSwitchLogIndex;
volatile int testsFinished;

void Task_switch(
  rtems_tcb *unused,
  rtems_tcb *heir
)
{
  uint32_t    index;
  rtems_time_of_day time;
  rtems_status_code status;

  index = task_number( heir->Object.id );

  switch( index ) {
    case 1:
    case 2:
    case 3:
      Run_count[ index ] += 1;

      status = rtems_clock_get_tod( &time );
      fatal_directive_status_with_level( status, RTEMS_SUCCESSFUL,
                                         "rtems_clock_get_tod", 1 );

      if (taskSwitchLogIndex <
          (sizeof taskSwitchLog / sizeof taskSwitchLog[0])) {
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

