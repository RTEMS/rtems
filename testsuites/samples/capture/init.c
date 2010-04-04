/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT

#include "system.h"
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/monitor.h>

volatile int can_proceed = 1;

rtems_task Init(
  rtems_task_argument ignored
)
{
#if BSP_SMALL_MEMORY
  printf("NO Capture Engine. MEMORY TOO SMALL");
#else
  rtems_task_priority old_priority;
  rtems_mode          old_mode;

  /* lower the task priority to allow created tasks to execute */

  rtems_task_set_priority(RTEMS_SELF, 20, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  printf( "\n*** CAPTURE ENGINE TEST ***\n" );

  while (!can_proceed)
  {
    printf ("Sleeping\n");
    usleep (1000000);
  }

  rtems_monitor_init (0);
  rtems_capture_cli_init (0);

  setup_tasks_to_watch ();

  rtems_task_delete (RTEMS_SELF);
#endif
}
