/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/threadsup.h>

void _POSIX_Threads_cancel_run(
  Thread_Control *the_thread
)
{
  POSIX_Cancel_Handler_control      *handler;
  Chain_Control                     *handler_stack;
  POSIX_API_Control                 *thread_support;
  ISR_Level                          level;

  thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];

  handler_stack = &thread_support->Cancellation_Handlers;

  thread_support->cancelability_state = PTHREAD_CANCEL_DISABLE;

  while ( !_Chain_Is_empty( handler_stack ) ) {
    _ISR_Disable( level );
      handler = (POSIX_Cancel_Handler_control *)
           _Chain_Tail( handler_stack )->previous;
      _Chain_Extract_unprotected( &handler->Node );
    _ISR_Enable( level );

    (*handler->routine)( handler->arg );

    _Workspace_Free( handler );
  }
}
