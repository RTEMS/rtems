/*
 *  Invoke the destructor on a per-task variable
 *
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/wkspace.h>

void _RTEMS_Tasks_Invoke_task_variable_dtor(
  Thread_Control        *the_thread,
  rtems_task_variable_t *tvp
)
{
  void (*dtor)(void *);
  void *value;

  dtor = tvp->dtor;
  if (_Thread_Is_executing(the_thread)) {
    value = *tvp->ptr;
    *tvp->ptr = tvp->gval;
  } else {
    value = tvp->tval;
  }

  if ( dtor )
    (*dtor)(value);

  _Workspace_Free(tvp);
}
