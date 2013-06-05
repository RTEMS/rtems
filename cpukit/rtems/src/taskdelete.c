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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/apimutex.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
#ifdef RTEMS_SMP
  return rtems_task_suspend( id );
#else /* RTEMS_SMP */
  register Thread_Control *the_thread;
  Objects_Locations        location;
  Objects_Information     *the_information;

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
#endif /* RTEMS_SMP */
}
