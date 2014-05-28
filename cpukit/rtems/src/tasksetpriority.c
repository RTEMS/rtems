/**
 *  @file
 *
 *  @brief RTEMS Set Task Priority
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
)
{
  Thread_Control          *the_thread;
  Objects_Locations        location;

  if ( new_priority != RTEMS_CURRENT_PRIORITY &&
       !_RTEMS_tasks_Priority_is_valid( new_priority ) )
    return RTEMS_INVALID_PRIORITY;

  if ( !old_priority )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      *old_priority = _RTEMS_tasks_Priority_from_Core(
                        the_thread->current_priority
                      );
      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        the_thread->real_priority = _RTEMS_tasks_Priority_to_Core(
                                      new_priority
                                    );
        if ( !_Thread_Owns_resources( the_thread ) ||
             the_thread->current_priority > new_priority )
          _Thread_Change_priority( the_thread, new_priority, false );
      }
      _Objects_Put( &the_thread->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = old_priority;
      return _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_SET_PRIORITY_REQUEST,
          id,
          new_priority,
          0,          /* Not used */
          0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
