/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
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
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel(
  pthread_t  thread
)
{
  Thread_Control     *the_thread;
  POSIX_API_Control  *thread_support;
  Objects_Locations   location;
  bool                cancel = false;

  /*
   *  Don't even think about deleting a resource from an ISR.
   */

  if ( _ISR_Is_in_progress() )
    return EPROTO;

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      thread_support = the_thread->API_Extensions[ THREAD_API_POSIX ];

      thread_support->cancelation_requested = 1;

      if (thread_support->cancelability_state == PTHREAD_CANCEL_ENABLE &&
          thread_support->cancelability_type == PTHREAD_CANCEL_ASYNCHRONOUS)
        cancel = true;

      _Thread_Enable_dispatch();
      if ( cancel )
        _POSIX_Thread_Exit( the_thread, PTHREAD_CANCELED );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
