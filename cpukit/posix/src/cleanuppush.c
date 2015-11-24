/**
 * @file
 *
 * @brief Establishing Cancellation Handlers
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
#include <rtems/posix/threadsup.h>

void _pthread_cleanup_push(
  struct _pthread_cleanup_context   *context,
  void                            ( *routine )( void * ),
  void                              *arg
)
{
  POSIX_API_Control *thread_support;

  context->_routine = routine;
  context->_arg = arg;

  /* This value is unused, just provide a deterministic value */
  context->_canceltype = -1;

  _Thread_Disable_dispatch();

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
  context->_previous = thread_support->last_cleanup_context;
  thread_support->last_cleanup_context = context;

  _Thread_Enable_dispatch();
}
