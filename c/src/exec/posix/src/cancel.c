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
 *  18.2.1 Canceling Execution of a Thread, P1003.1c/Draft 10, p. 181
 */

int pthread_cancel(
  pthread_t  thread
)
{
  Thread_Control                    *the_thread;
  POSIX_API_Control                 *thread_support;
  Objects_Locations                  location;

  the_thread = _POSIX_Threads_Get( thread, &location );
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
