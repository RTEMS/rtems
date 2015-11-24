/**
 * @file
 *
 * @brief Executes a thread's cancellation handlers
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/threadsup.h>

void _POSIX_Threads_cancel_run(
  Thread_Control *the_thread
)
{
  struct _pthread_cleanup_context *context;
  POSIX_API_Control               *thread_support;

  _Thread_Disable_dispatch();

  thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];
  thread_support->cancelability_state = PTHREAD_CANCEL_DISABLE;

  context = thread_support->last_cleanup_context;
  thread_support->last_cleanup_context = NULL;

  _Thread_Enable_dispatch();

  while ( context != NULL ) {
    ( *context->_routine )( context->_arg );

    context = context->_previous;
  }
}
