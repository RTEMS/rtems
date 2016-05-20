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
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Per_CPU_Control  *cpu_self;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  executing = _Per_CPU_Get_executing( cpu_self );

  if ( the_thread == executing ) {
    /*
     * The Classic tasks are neither detached nor joinable.  In case of
     * self deletion, they are detached, otherwise joinable by default.
     */
    _Thread_Exit(
      executing,
      THREAD_LIFE_TERMINATING | THREAD_LIFE_DETACHED,
      NULL
    );
  } else {
    _Thread_Close( the_thread, executing );
  }

  _Thread_Dispatch_enable( cpu_self );
  return RTEMS_SUCCESSFUL;
}
