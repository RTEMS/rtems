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

int pthread_setschedprio( pthread_t thread, int prio )
{
  Thread_Control    *the_thread;
  Per_CPU_Control   *cpu_self;
  POSIX_API_Control *api;
  Priority_Control   unused;
  ISR_lock_Context   lock_context;
  Priority_Control   new_priority;

  if ( !_POSIX_Priority_Is_valid( prio ) ) {
    return EINVAL;
  }

  new_priority = _POSIX_Priority_To_core( prio );

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );

  _Thread_State_acquire_critical( the_thread, &lock_context );
  api->Attributes.schedparam.sched_priority = prio;
  _Thread_State_release( the_thread, &lock_context );

  _Thread_Set_priority( the_thread, new_priority, &unused, true );

  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
