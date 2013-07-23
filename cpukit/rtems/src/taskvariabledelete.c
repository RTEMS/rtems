/**
 *  @file
 *
 *  @brief RTEMS Delete Task Variable
 *  @ingroup ClassicTasks
 */

/*
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
#include <rtems/config.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/wkspace.h>

rtems_status_code rtems_task_variable_delete(
  rtems_id  tid,
  void    **ptr
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  rtems_task_variable_t *tvp, *prev;

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  if ( !ptr )
    return RTEMS_INVALID_ADDRESS;

  prev = NULL;

  the_thread = _Thread_Get (tid, &location);
  switch (location) {

    case OBJECTS_LOCAL:
      tvp = the_thread->task_variables;
      while (tvp) {
        if (tvp->ptr == ptr) {
          if (prev)
            prev->next = tvp->next;
          else
            the_thread->task_variables = (rtems_task_variable_t *)tvp->next;

          _RTEMS_Tasks_Invoke_task_variable_dtor( the_thread, tvp );
          _Objects_Put( &the_thread->Object );
          return RTEMS_SUCCESSFUL;
        }
        prev = tvp;
        tvp = (rtems_task_variable_t *)tvp->next;
      }
      _Objects_Put( &the_thread->Object );
      return RTEMS_INVALID_ADDRESS;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
        break;
  }

  return RTEMS_INVALID_ID;
}
