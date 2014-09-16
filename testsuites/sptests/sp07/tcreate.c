/*  Task_create_extension
 *
 *  This routine is the tcreate user extension.
 *
 *  Input parameters:
 *    unused       - pointer to currently running TCB
 *    created_task - pointer to new TCB being created
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

bool Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
)
{
  ssize_t task = task_number( created_task->Object.id );

  if (task > 0) {
    ++Task_created[ task ];
  }

  return true;
}
