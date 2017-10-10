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

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

int pthread_setaffinity_np(
  pthread_t        thread,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;
  bool              ok;

  if ( cpuset == NULL ) {
    return EFAULT;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_acquire_critical( the_thread, &lock_context );

  ok = _Scheduler_Set_affinity(
    the_thread,
    cpusetsize,
    cpuset
  );

  _Thread_State_release( the_thread, &lock_context );
  _Thread_Dispatch_enable( cpu_self );
  return ok ? 0 : EINVAL;
}
#endif
