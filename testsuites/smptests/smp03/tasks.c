/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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

  /* If this is the last task created set a flag for the
   * test to end.
   */
  if ( task_index  == rtems_smp_get_number_of_processors() ) {
    TestFinished = true;
  }

  /* Wait for the test to end without giving up this processor */
  while(1)
    ;
}
