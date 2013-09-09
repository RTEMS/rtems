/**
 *  @file
 *
 *  @brief RTEMS Delete Task
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/apimutex.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  Objects_Information     *the_information;

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  _RTEMS_Lock_allocator();

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      the_information = _Objects_Get_information_id( the_thread->Object.id );

      #if defined(RTEMS_DEBUG)
        if ( !the_information ) {
          _Objects_Put( &the_thread->Object );
          return RTEMS_INVALID_ID;
          /* This should never happen if _Thread_Get() works right */
        }
      #endif

      #if defined(RTEMS_MULTIPROCESSING)
        if ( the_thread->is_global ) {
          _Objects_MP_Close( &_RTEMS_tasks_Information, the_thread->Object.id );
          _RTEMS_tasks_MP_Send_process_packet(
            RTEMS_TASKS_MP_ANNOUNCE_DELETE,
            the_thread->Object.id,
            0                                /* Not used */
          );
        }
      #endif

      _Thread_Close( the_information, the_thread );

      _RTEMS_tasks_Free( the_thread );

      /* FIXME: Lock order reversal */
      _RTEMS_Unlock_allocator();
      _Objects_Put( &the_thread->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _RTEMS_Unlock_allocator();
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  _RTEMS_Unlock_allocator();
  return RTEMS_INVALID_ID;
}
