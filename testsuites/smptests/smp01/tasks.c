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

#include <inttypes.h>

rtems_task Test_task(
  rtems_task_argument task_index
)
{
  uint32_t          cpu_num;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

   /* Get the CPU Number */
  cpu_num = rtems_smp_get_current_processor();

  /* Print that the task is up and running. */
  Loop();
  locked_printf(" CPU %" PRIu32 " running Task %s\n", cpu_num, name);

  /* Set the flag that the task is up and running */
  TaskRan[cpu_num] = true;


  /* Drop into a loop which will keep this task on
   * running on the cpu.
   */
  while(1);
}
