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
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcancelstate( 
  int  state,
  int *oldstate
)
{
  POSIX_API_Control                 *thread_support;

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
    _POSIX_Thread_cancel_run( _Thread_Executing );
 
  return 0;
}
