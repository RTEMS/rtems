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

#ifndef HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT

#include <rtems/score/chainimpl.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthreadimpl.h>

/*
 *  18.2.3.1 Establishing Cancellation Handlers, P1003.1c/Draft 10, p. 184
 */

void pthread_cleanup_push(
  void   (*routine)( void * ),
  void    *arg
)
{
  POSIX_Cancel_Handler_control      *handler;
  Chain_Control                     *handler_stack;
  POSIX_API_Control                 *thread_support;

  /*
   *  The POSIX standard does not address what to do when the routine
   *  is NULL.  It also does not address what happens when we cannot
   *  allocate memory or anything else bad happens.
   */
  if ( !routine )
    return;

  _Thread_Disable_dispatch();
  handler = _Workspace_Allocate( sizeof( POSIX_Cancel_Handler_control ) );

  if ( handler ) {
    thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

    handler_stack = &thread_support->Cancellation_Handlers;

    handler->routine = routine;
    handler->arg = arg;

    _Chain_Append( handler_stack, &handler->Node );
  }
  _Thread_Enable_dispatch();
}

#else /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */

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

#endif /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */
