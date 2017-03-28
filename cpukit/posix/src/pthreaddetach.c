/**
 * @file
 *
 * @brief Detaching a Thread
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/score/threadimpl.h>

/**
 * 16.1.4 Detaching a Thread, P1003.1c/Draft 10, p. 149
 */
int pthread_detach( pthread_t thread )
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  the_thread->Life.state |= THREAD_LIFE_DETACHED;
  _Thread_Clear_state_locked( the_thread, STATES_WAITING_FOR_JOIN_AT_EXIT );

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_release( the_thread, &lock_context );
  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
