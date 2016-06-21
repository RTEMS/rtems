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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

typedef struct {
  const Scheduler_Control *scheduler;
  rtems_task_priority      new_priority;
  Priority_Control         old_priority;
  rtems_status_code        status;
} RTEMS_tasks_Set_priority_context;

static bool _RTEMS_tasks_Set_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_p,
  void             *arg
)
{
  RTEMS_tasks_Set_priority_context *context;
  const Scheduler_Control          *scheduler;
  bool                              valid;
  Priority_Control                  current_priority;
  Priority_Control                  new_priority;

  context = arg;
  scheduler = _Scheduler_Get_own( the_thread );
  current_priority = the_thread->current_priority;

  context->scheduler = scheduler;
  context->old_priority = current_priority;

  new_priority = _RTEMS_Priority_To_core(
    scheduler,
    context->new_priority,
    &valid
  );

  *new_priority_p = new_priority;

  if ( !valid ) {
    context->status = RTEMS_INVALID_PRIORITY;
    return false;
  }

  the_thread->real_priority = new_priority;
  context->status = STATUS_SUCCESSFUL;

  return _Thread_Priority_less_than( current_priority, new_priority )
    || !_Thread_Owns_resources( the_thread );
}

rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority_p
)
{
  Thread_Control          *the_thread;
  ISR_lock_Context         lock_context;
  const Scheduler_Control *scheduler;
  Priority_Control         old_priority;
  rtems_status_code        status;

  if ( old_priority_p == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _RTEMS_tasks_MP_Set_priority( id, new_priority, old_priority_p );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
    RTEMS_tasks_Set_priority_context  context;
    Per_CPU_Control                  *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _ISR_lock_ISR_enable( &lock_context );

    context.new_priority = new_priority;
    _Thread_Change_priority(
      the_thread,
      0,
      &context,
      _RTEMS_tasks_Set_priority_filter,
      false
    );

    _Thread_Dispatch_enable( cpu_self );
    scheduler = context.scheduler;
    old_priority = context.old_priority;
    status = context.status;
  } else {
    _Thread_State_acquire_critical( the_thread, &lock_context );
    scheduler = _Scheduler_Get_own( the_thread );
    old_priority = the_thread->current_priority;
    _Thread_State_release( the_thread, &lock_context );
    status = RTEMS_SUCCESSFUL;
  }

  *old_priority_p = _RTEMS_Priority_From_core( scheduler, old_priority );
  return status;
}
