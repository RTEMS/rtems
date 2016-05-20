/**
 * @file
 *
 * @brief Sends a signal Asynchronously directed to a thread
 * @ingroup POSIXAPI
 */

/*
 *  3.3.10 Send a Signal to a Thread, P1003.1c/D10, p. 43
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_kill( pthread_t thread, int sig )
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  POSIX_API_Control *api;
  Per_CPU_Control   *cpu_self;

  if ( !is_valid_signo( sig ) ) {
    return EINVAL;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  if ( _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN ) {
    _ISR_lock_ISR_enable( &lock_context );
    return 0;
  }

  /* XXX critical section */

  api->signals_pending |= signo_to_mask( sig );

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  (void) _POSIX_signals_Unblock_thread( the_thread, sig, NULL );
  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
