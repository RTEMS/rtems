/**
 * @file
 *
 * @brief rtems_task_is_suspended
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

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>

/*
 *  rtems_task_is_suspended
 *
 *  This directive returns a status indicating whether or not
 *  the specified task is suspended.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL        - if successful and not suspended
 *    RTEMS_ALREADY_SUSPENDED - if successful and suspended
 *    error code              - if unsuccessful
 */

rtems_status_code rtems_task_is_suspended(
  rtems_id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( !_States_Is_suspended( the_thread->current_state ) ) {
        _Objects_Put( &the_thread->Object );
        return RTEMS_SUCCESSFUL;
      }
      _Objects_Put( &the_thread->Object );
      return RTEMS_ALREADY_SUSPENDED;

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
