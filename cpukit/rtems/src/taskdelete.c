/**
 *  @file
 *
 *  @brief RTEMS Delete Task
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
#include <rtems/score/apimutex.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;
  bool               previous_life_protection;

  previous_life_protection = _Thread_Set_life_protection( true );
  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      #if defined(RTEMS_MULTIPROCESSING)
        if ( the_thread->is_global ) {
          _Objects_MP_Close(
            &_RTEMS_tasks_Information.Objects,
            the_thread->Object.id
          );
          _RTEMS_tasks_MP_Send_process_packet(
            RTEMS_TASKS_MP_ANNOUNCE_DELETE,
            the_thread->Object.id,
            0                                /* Not used */
          );
        }
      #endif

      _Thread_Close( the_thread, _Thread_Executing );

      _Objects_Put( &the_thread->Object );
      _Thread_Set_life_protection( previous_life_protection );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      _Thread_Set_life_protection( previous_life_protection );
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  _Thread_Set_life_protection( previous_life_protection );

  return RTEMS_INVALID_ID;
}
