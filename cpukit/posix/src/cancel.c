/*  cancel.c
 *
 *  $Id$
 */

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
 *  POSIX_Thread_cancel_run
 *
 */

void POSIX_Thread_cancel_run( 
  Thread_Control *the_thread
)
{
  int                                old_cancel_state;
  POSIX_Cancel_Handler_control      *handler;
  Chain_Control                     *handler_stack;
  POSIX_API_Thread_Support_Control  *thread_support;
  ISR_Level                          level;
 
  thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];
 
  handler_stack = &thread_support->Cancellation_Handlers;
 
  old_cancel_state = thread_support->cancelability_state;

  thread_support->cancelability_state = PTHREAD_CANCEL_DISABLE;

  while ( !_Chain_Is_empty( handler_stack ) ) {
    _ISR_Disable( level );
      handler = (POSIX_Cancel_Handler_control *) _Chain_Tail( handler_stack );
      _Chain_Extract_unprotected( &handler->Node );
    _ISR_Enable( level );
 
    (*handler->routine)( handler->arg );

    _Workspace_Free( handler );
  }

  thread_support->cancelation_requested = 0;

  thread_support->cancelability_state = old_cancel_state;
}

/*PAGE
 *
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel(
  pthread_t  thread
)
{
  Thread_Control                    *the_thread;
  POSIX_API_Thread_Support_Control  *thread_support;
  Objects_Locations                  location;

  the_thread = _POSIX_Threads_Get( &thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_REMOTE:
      return POSIX_MP_NOT_IMPLEMENTED();
    case OBJECTS_LOCAL:
      thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];

      thread_support->cancelation_requested = 1;

      _Thread_Enable_dispatch();
      return 0;
  }
 
  return POSIX_BOTTOM_REACHED();
}

/*PAGE
 *
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcancelstate( 
  int  state,
  int *oldstate
)
{
  POSIX_API_Thread_Support_Control  *thread_support;

  if ( !oldstate )
    return EINVAL;

  if ( state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE )
    return EINVAL;

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
 
  *oldstate = thread_support->cancelability_state;
  thread_support->cancelability_state = state;
 
  if ( thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE && 
       thread_support->cancelability_type == PTHREAD_CANCEL_ASYNCHRONOUS &&
       thread_support->cancelation_requested )
    POSIX_Thread_cancel_run( _Thread_Executing );
 
  return 0;
}

/*PAGE
 *
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcanceltype(
  int  type,
  int *oldtype
)
{
  POSIX_API_Thread_Support_Control  *thread_support;
 
  if ( !oldtype )
    return EINVAL;
 
  if ( type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS )
    return EINVAL;
 
  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  *oldtype = thread_support->cancelability_type;
  thread_support->cancelability_type = type;

  if ( thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE && 
       thread_support->cancelability_type == PTHREAD_CANCEL_ASYNCHRONOUS &&
       thread_support->cancelation_requested )
    POSIX_Thread_cancel_run( _Thread_Executing );

  return 0;
}

/*PAGE
 *
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

void pthread_testcancel( void )
{
  POSIX_API_Thread_Support_Control  *thread_support;

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
 
  if ( thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE &&
       thread_support->cancelation_requested )
    POSIX_Thread_cancel_run( _Thread_Executing );
}

/*PAGE
 *
 *  18.2.3.1 Establishing Cancellation Handlers, P1003.1c/Draft 10, p. 184
 */

void pthread_cleanup_push(
  void   (*routine)( void * ),
  void    *arg
)
{
  POSIX_Cancel_Handler_control      *handler;
  Chain_Control                     *handler_stack;
  POSIX_API_Thread_Support_Control  *thread_support;
  
  if ( !routine )
    return;          /* XXX what to do really? */ 

  handler = _Workspace_Allocate( sizeof( POSIX_Cancel_Handler_control ) );

  if ( !handler )
    return;          /* XXX what to do really? */ 

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  handler_stack = &thread_support->Cancellation_Handlers;

  handler->routine = routine;
  handler->arg = arg;

  _Chain_Append( handler_stack, &handler->Node );
}

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
  POSIX_API_Thread_Support_Control  *thread_support;
  ISR_Level                          level;
 
  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
 
  handler_stack = &thread_support->Cancellation_Handlers;

  if ( _Chain_Is_empty( handler_stack ) )
    return;
 
  _ISR_Disable( level );
    handler = (POSIX_Cancel_Handler_control *) _Chain_Tail( handler_stack );
    _Chain_Extract_unprotected( &handler->Node );
  _ISR_Enable( level );

  if ( execute )
    (*handler->routine)( handler->arg );
 
  _Workspace_Free( handler );
}
