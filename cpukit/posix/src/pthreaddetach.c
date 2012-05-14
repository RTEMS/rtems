/*
 *  16.1.4 Detaching a Thread, P1003.1c/Draft 10, p. 149
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>

int pthread_detach(
  pthread_t   thread
)
{
  register Thread_Control *the_thread;
  POSIX_API_Control       *api;
  Objects_Locations        location;

  the_thread = _Thread_Get( thread, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];
      api->detachstate = PTHREAD_CREATE_DETACHED;
      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
