/*
 *  $Id$
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

/*PAGE
 *
 *  18.2.3.1 Establishing Cancellation Handlers, P1003.1c/Draft 10, p. 184
 */

void pthread_cleanup_pop(
  int    execute
)
{
  POSIX_Cancel_Handler_control      *handler;
  Chain_Control                     *handler_stack;
  POSIX_API_Control                 *thread_support;
  ISR_Level                          level;
 
  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
 
  handler_stack = &thread_support->Cancellation_Handlers;

  if ( _Chain_Is_empty( handler_stack ) )
    return;
 
  _ISR_Disable( level );
    handler = (POSIX_Cancel_Handler_control *) 
        _Chain_Tail( handler_stack )->previous;
    _Chain_Extract_unprotected( &handler->Node );
  _ISR_Enable( level );

  if ( execute )
    (*handler->routine)( handler->arg );
 
  _Workspace_Free( handler );
}
