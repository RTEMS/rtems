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

/*
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel(
  pthread_t  thread
)
{
  Thread_Control     *the_thread;
  POSIX_API_Control  *thread_support;
  Objects_Locations   location;

  /*
   *  Don't even think about deleting a resource from an ISR.
   */

  if ( _ISR_Is_in_progress() )
    return EPROTO;

  the_thread = _Thread_Get( thread, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];

      thread_support->cancelation_requested = 1;

      /* This enables dispatch implicitly */
      _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch( the_thread );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
