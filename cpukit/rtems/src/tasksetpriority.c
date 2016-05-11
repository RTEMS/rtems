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
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;

  if ( new_priority != RTEMS_CURRENT_PRIORITY &&
       !_RTEMS_tasks_Priority_is_valid( new_priority ) )
    return RTEMS_INVALID_PRIORITY;

  if ( !old_priority )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get_interrupt_disable( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _RTEMS_tasks_MP_Set_priority( id, new_priority, old_priority );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
    _Thread_Set_priority(
      the_thread,
      _RTEMS_tasks_Priority_to_Core( new_priority ),
      old_priority,
      false
    );
    *old_priority = _RTEMS_tasks_Priority_from_Core( *old_priority );
  } else {
    *old_priority = _RTEMS_tasks_Priority_from_Core(
      the_thread->current_priority
    );
  }

  _Thread_Dispatch_enable( cpu_self );
  return RTEMS_SUCCESSFUL;
}
