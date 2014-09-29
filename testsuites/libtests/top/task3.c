/*  Task_3
 *
 *  This routine serves as a test task.  It receives a task
 *  number and uses it to force each progressive task created
 *  to have a higher cpu utilization than the previous.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_3(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t sum;
  uint32_t next;
  uint32_t per_loop;

  sum  = 0;
  next = 0;
  per_loop = argument * 1000;

  while( FOREVER ) {
    add_some( per_loop, &sum, &next );

    status = rtems_task_wake_after (TicksPerSecond * 1);
    directive_failed( status, "rtems_task_wake_after" );
 }
}
