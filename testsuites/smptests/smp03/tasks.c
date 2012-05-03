/*
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

rtems_task Test_task(
  rtems_task_argument task_index
)
{
  char              task_name[5];

  /* Show that this task is running on cpu X */
  sprintf( task_name, "TA%" PRIu32, task_index );
  PrintTaskInfo( task_name );

  TaskRan[task_index] = true;

  /* Wait for the test to end without giving up this processor */
  while(1)
    ;
}
