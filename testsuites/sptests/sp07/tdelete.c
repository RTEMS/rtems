/*  Task_delete_extension
 *
 *  This routine is the delete user extension.
 *
 *  Input parameters:
 *    running_task - pointer to running TCB
 *    deleted_task - pointer to TCB deleted
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
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

void Task_delete_extension(
  rtems_tcb *running_task,
  rtems_tcb *deleted_task
)
{
  ssize_t task = task_number( deleted_task->Object.id );

  if (task > 0) {
    ++Task_deleted[ task ];
  }

  rtems_test_assert( running_task != deleted_task );
}
