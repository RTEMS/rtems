/*  Task_2
 *
 *  This routine serves as a test task.  It is just a CPU bound task
 *  requiring timesliced operation.
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include <rtems/cpuuse.h>

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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  while ( !testsFinished );

  showTaskSwitches ();
  puts( "" );
  rtems_cpu_usage_report();
  puts( "" );
  puts( "TA2 - RESETTING USAGE STATISTICS" );
  rtems_cpu_usage_reset();
  puts( "" );
  rtems_cpu_usage_report();
  puts( "" );
  puts( "*** END OF CPU USAGE LIBRARY TEST ***" );
  rtems_test_exit( 0 );
}
