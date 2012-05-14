/*
 *
 *  COPYRIGHT (c) 1989-2011.
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
 *  Only require 10 task switches on sis/gdb.  --joel 2 Feb 2011
 */
#define MAX_TASK_SWITCH_LOGS 25

struct taskSwitchLog taskSwitchLog[MAX_TASK_SWITCH_LOGS];
unsigned int taskSwitchLogIndex;
volatile int testsFinished;

void Task_switch(
  rtems_tcb *unused,
  rtems_tcb *heir
)
{
  uint32_t    index;
  rtems_time_of_day time;
  rtems_status_code status;

  index = task_number( heir->Object.id ) - task_number( Task_id[1] ) + 1;

  switch( index ) {
    case 1:
    case 2:
    case 3:
      Run_count[ index ] += 1;

      status = rtems_clock_get_tod( &time );
      directive_failed_with_level( status, "rtems_clock_get_tod", 1 );

      if (taskSwitchLogIndex < MAX_TASK_SWITCH_LOGS ) {
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
