/**
 * @file
 *
 * @brief Pthread Set Affinity
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_SETAFFINITY_NP

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/schedulerimpl.h>

int pthread_setaffinity_np(
  pthread_t          id,
  size_t             cpusetsize,
  const cpu_set_t   *cpuset)
{
  Objects_Locations        location;
  POSIX_API_Control       *api;
  Thread_Control          *the_thread;
  bool                     ok;

  if ( !cpuset )
    return EFAULT;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      ok = _Scheduler_Set_affinity(
        the_thread,
        cpusetsize,
        cpuset
      );
      if ( ok ) {
        api = the_thread->API_Extensions[ THREAD_API_POSIX ];
        CPU_COPY( api->Attributes.affinityset, cpuset );
      }
      _Objects_Put( &the_thread->Object );
      return ok ? 0 : EINVAL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
#endif
