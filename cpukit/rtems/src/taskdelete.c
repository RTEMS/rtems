/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_delete().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_delete(
  rtems_id id
)
{
  Thread_Control       *the_thread;
  Thread_Close_context  context;
  Per_CPU_Control      *cpu_self;
  Thread_Control       *executing;

  _Thread_queue_Context_initialize( &context.Base );
  the_thread = _Thread_Get( id, &context.Base.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  cpu_self = _Per_CPU_Get();

  if ( _Per_CPU_Is_ISR_in_progress( cpu_self ) ) {
    _ISR_lock_ISR_enable( &context.Base.Lock_context.Lock_context );
    return RTEMS_CALLED_FROM_ISR;
  }

  executing = _Per_CPU_Get_executing( cpu_self );

  if ( the_thread == executing ) {
    _ISR_lock_ISR_enable( &context.Base.Lock_context.Lock_context );

    /*
     * The Classic tasks are neither detached nor joinable.  In case of
     * self deletion, they are detached, otherwise joinable by default.
     */
    _Thread_Exit( NULL, THREAD_LIFE_TERMINATING | THREAD_LIFE_DETACHED );
  } else {
    _Thread_Close( the_thread, executing, &context );
  }

  return RTEMS_SUCCESSFUL;
}
