/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Pthread Get Affinity
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define  _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_DECL_PTHREAD_GETAFFINITY_NP

#include <pthread.h>
#include <errno.h>

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_getaffinity_np(
  pthread_t  thread,
  size_t     cpusetsize,
  cpu_set_t *cpuset
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Status_Control    status;

  if ( cpuset == NULL ) {
    return EFAULT;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  status = _Scheduler_Get_affinity(
    the_thread,
    cpusetsize,
    cpuset
  );

  _Thread_State_release( the_thread, &lock_context );
  return _POSIX_Get_error( status );
}

#endif
