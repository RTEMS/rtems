/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

int pthread_setschedprio( pthread_t thread, int prio )
{
  Thread_Control          *the_thread;
  Per_CPU_Control         *cpu_self;
  Thread_queue_Context     queue_context;
  const Scheduler_Control *scheduler;
  Priority_Control         new_priority;
  bool                     valid;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  the_thread = _Thread_Get( thread, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_Wait_acquire_critical( the_thread, &queue_context );

  scheduler = _Thread_Scheduler_get_home( the_thread );

  new_priority = _POSIX_Priority_To_core( scheduler, prio, &valid );
  if ( !valid ) {
    _Thread_Wait_release( the_thread, &queue_context );
    return EINVAL;
  }

  _Thread_Priority_change(
    the_thread,
    &the_thread->Real_priority,
    new_priority,
    true,
    &queue_context
  );

  cpu_self = _Thread_queue_Dispatch_disable( &queue_context );
  _Thread_Wait_release( the_thread, &queue_context );

  _Thread_Priority_update( &queue_context );

  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
