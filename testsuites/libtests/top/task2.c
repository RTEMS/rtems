/*  Task_2
 *
 *  This routine serves as a test task.  It calls the
 *  top command and forces an exit if the user requests it.
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

#include <rtems/cpuuse.h>

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_cpu_usage_top();
  TEST_END();
  rtems_test_exit( 0 );
}
