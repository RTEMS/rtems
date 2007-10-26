/*
 *  rtems_task_variable_delete - Delete a per-task variable
 *
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/wkspace.h>

/*
 *  rtems_task_variable_delete
 *
 *  This directive removes a task variable.
 */

rtems_status_code rtems_task_variable_delete(
  rtems_id  tid,
  void    **ptr
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  rtems_task_variable_t *tvp, *prev;

  if ( !ptr )
    return RTEMS_INVALID_ADDRESS;

  prev = NULL;

  the_thread = _Thread_Get (tid, &location);
  switch (location) {
#if defined(RTEMS_MULTIPROCESSING)
  case OBJECTS_REMOTE:
    _Thread_Dispatch();
    return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif


  case OBJECTS_ERROR:
  default:
    return RTEMS_INVALID_ID;

  case OBJECTS_LOCAL:
    tvp = the_thread->task_variables;
    while (tvp) {
      if (tvp->ptr == ptr) {
        if (prev)
          prev->next = tvp->next;
        else      
          the_thread->task_variables = (rtems_task_variable_t *)tvp->next;
        if (_Thread_Is_executing(the_thread)) {
          if (tvp->dtor)
            (*tvp->dtor)(*tvp->ptr);
          *tvp->ptr = tvp->gval;
	} else {
          if (tvp->dtor)
            (*tvp->dtor)(tvp->tval);
        }
        _Workspace_Free(tvp);
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      prev = tvp;
      tvp = (rtems_task_variable_t *)tvp->next;
    }
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
