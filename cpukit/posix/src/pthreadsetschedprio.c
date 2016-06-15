/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

typedef struct {
  int prio;
  int error;
} POSIX_Set_sched_prio_context;

static bool _POSIX_Set_sched_prio_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_p,
  void             *arg
)
{
  POSIX_Set_sched_prio_context *context;
  int                           prio;
  const Scheduler_Control      *scheduler;
  POSIX_API_Control            *api;
  bool                          valid;
  Priority_Control              current_priority;
  Priority_Control              new_priority;

  context = arg;
  prio = context->prio;
  scheduler = _Scheduler_Get_own( the_thread );

  new_priority = _POSIX_Priority_To_core( scheduler, prio, &valid );
  if ( !valid ) {
    context->error = EINVAL;
    return false;
  }

  *new_priority_p = new_priority;

  current_priority = the_thread->current_priority;
  the_thread->real_priority = new_priority;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  api->Sporadic.high_priority = new_priority;

  if ( api->Sporadic.low_priority < new_priority ) {
    api->Sporadic.low_priority  = new_priority;
  }

  context->error = 0;
  return _Thread_Priority_less_than( current_priority, new_priority )
    || !_Thread_Owns_resources( the_thread );
}

int pthread_setschedprio( pthread_t thread, int prio )
{
  Thread_Control               *the_thread;
  Per_CPU_Control              *cpu_self;
  POSIX_Set_sched_prio_context  context;
  ISR_lock_Context              lock_context;

  context.prio = prio;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  _Thread_Change_priority(
    the_thread,
    0,
    &context,
    _POSIX_Set_sched_prio_filter,
    true
  );

  _Thread_Dispatch_enable( cpu_self );
  return context.error;
}
