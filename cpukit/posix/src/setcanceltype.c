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

int pthread_setcanceltype(
  int  type,
  int *oldtype
)
{
  POSIX_API_Control                 *thread_support;
 
  /*
   *  Don't even think about deleting a resource from an ISR.
   *  Besides this request is supposed to be for _Thread_Executing
   *  and the ISR context is not a thread.
   */

  if ( _ISR_Is_in_progress() ) 
    return EPROTO;

  if ( !oldtype )
    return EINVAL;
 
  if ( type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS )
    return EINVAL;
 
  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  _Thread_Disable_dispatch();
    *oldtype = thread_support->cancelability_type;
    thread_support->cancelability_type = type;

    if ( thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE && 
         thread_support->cancelability_type == PTHREAD_CANCEL_ASYNCHRONOUS &&
         thread_support->cancelation_requested )
      _POSIX_Threads_cancel_run( _Thread_Executing );
  _Thread_Enable_dispatch();

  return 0;
}
