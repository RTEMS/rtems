/**
 * @file
 *
 * @brief Get a per-task variable
 * @ingroup ClassicTasks Tasks
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

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

/*
 *  rtems_task_variable_get
 *
 *  This directive gets the value of a task variable.
 */

rtems_status_code rtems_task_variable_get(
  rtems_id tid,
  void **ptr,
  void **result
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  rtems_task_variable_t *tvp;

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  if ( !ptr )
    return RTEMS_INVALID_ADDRESS;

  if ( !result )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get (tid, &location);
  switch (location) {

    case OBJECTS_LOCAL:
      /*
       *  Figure out if the variable is in this task's list.
       */
      tvp = the_thread->task_variables;
      while (tvp) {
        if (tvp->ptr == ptr) {
	  /*
	   * Should this return the current (i.e not the
	   * saved) value if `tid' is the current task?
	   */
          *result = tvp->tval;
          _Objects_Put( &the_thread->Object );
          return RTEMS_SUCCESSFUL;
        }
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
