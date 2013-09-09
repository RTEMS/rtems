/**
 *  @file
 *
 *  @brief RTEMS Suspend Task
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

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_suspend(
  rtems_id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( !_States_Is_suspended( the_thread->current_state ) ) {
        _Thread_Suspend( the_thread );
        _Objects_Put( &the_thread->Object );
        return RTEMS_SUCCESSFUL;
      }
      _Objects_Put( &the_thread->Object );
      return RTEMS_ALREADY_SUSPENDED;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_SUSPEND_REQUEST,
        id,
        0,          /* Not used */
        0,          /* Not used */
        0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
