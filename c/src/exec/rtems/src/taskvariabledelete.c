/*
 *  rtems_task_variable_delete - Delete a per-task variable
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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
  int      *ptr
)
{
  Thread_Control               *the_thread;
  Objects_Locations             location;
  RTEMS_API_Control            *api;
  struct rtems_task_variable_t *tvp, *prev;

  prev = NULL;

  the_thread = _Thread_Get (tid, &location);
  switch (location) {
  case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
    _Thread_Dispatch();
    return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

  case OBJECTS_ERROR:
    return RTEMS_INVALID_ID;

  default:
    return RTEMS_INTERNAL_ERROR;

  case OBJECTS_LOCAL:
    api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
    tvp = api->task_variables;
    while (tvp) {
      if (tvp->ptr == ptr) {
        if (prev) prev->next = tvp->next;
        else      api->task_variables = tvp->next;
        _Thread_Enable_dispatch();
        _Workspace_Free(tvp);
        return RTEMS_SUCCESSFUL;
      }
      prev = tvp;
      tvp = tvp->next;
    }
    _Thread_Enable_dispatch();
    return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
