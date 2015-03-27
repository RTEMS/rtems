/**
 *  @file
 *
 *  @brief RTEMS Resume Task
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

rtems_status_code rtems_task_resume(
  rtems_id id
)
{
  Thread_Control          *the_thread;
  Objects_Locations        location;
  States_Control           previous_state;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      previous_state = _Thread_Clear_state( the_thread, STATES_SUSPENDED );
      _Objects_Put( &the_thread->Object );

      return _States_Is_suspended( previous_state ) ?
        RTEMS_SUCCESSFUL : RTEMS_INCORRECT_STATE;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_RESUME_REQUEST,
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
