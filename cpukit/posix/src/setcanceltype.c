/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/posix/cancel.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/threadsup.h>

/*
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcanceltype(
  int  type,
  int *oldtype
)
{
  POSIX_API_Control *thread_support;

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

    _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch(_Thread_Executing);

  /*
   *  _Thread_Enable_dispatch is invoked by above call.
   */
  return 0;
}
