/*
 *  13.5.2 Dynamic Thread Scheduling Parameters Access,
 *         P1003.1c/Draft 10, p. 124
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>

int pthread_getschedparam(
  pthread_t           thread,
  int                *policy,
  struct sched_param *param
)
{
  Objects_Locations        location;
  POSIX_API_Control       *api;
  register Thread_Control *the_thread;

  if ( !policy || !param  )
    return EINVAL;

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      return ESRCH;
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];
      if ( policy )
        *policy = api->schedpolicy;
      if ( param ) {
        *param  = api->schedparam;
        param->sched_priority =
          _POSIX_Priority_From_core( the_thread->current_priority );
      }
      _Thread_Enable_dispatch();
      return 0;
  }

  return POSIX_BOTTOM_REACHED();

}
