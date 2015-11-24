/**
 * @file
 *
 * @brief Removes Routine from Top of Calling Thread's stack and Invoke it 
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

void _pthread_cleanup_pop(
  struct _pthread_cleanup_context *context,
  int                              execute
)
{
  POSIX_API_Control *thread_support;

  if ( execute != 0 ) {
    ( *context->_routine )( context->_arg );
  }

  _Thread_Disable_dispatch();

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
  thread_support->last_cleanup_context = context->_previous;

  _Thread_Enable_dispatch();
}
