/**
 * @file
 *
 * @brief Pthread Get Affinity
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

#if HAVE_DECL_PTHREAD_GETAFFINITY_NP

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

int pthread_getaffinity_np(
  const pthread_t       id,
  size_t                cpusetsize,
  cpu_set_t            *cpuset
)
{
  Objects_Locations        location;
  Thread_Control          *the_thread;
  bool                     ok;

  if ( !cpuset )
    return EFAULT;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      ok = _Scheduler_Get_affinity(
        the_thread,
        cpusetsize,
        cpuset
      );
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
