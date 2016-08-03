/**
 * @file
 *
 * @brief Compare Thread IDs
 * @ingroup POSIXAPI
 */

/*
 *  NOTE:  POSIX does not define the behavior when either thread id is invalid.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_equal(
  pthread_t  t1,
  pthread_t  t2
)
{
  /*
   *  If the system is configured for debug, then we will do everything we
   *  can to insure that both ids are valid.  Otherwise, we will do the
   *  cheapest possible thing to determine if they are equal.
   */

#ifndef RTEMS_DEBUG
  return _Objects_Are_ids_equal( t1, t2 );
#else
  ISR_lock_Context  lock_context_1;
  ISR_lock_Context  lock_context_2;
  Thread_Control   *thread_1;
  Thread_Control   *thread_2;

  thread_1 = _Thread_Get( t1, &lock_context_1 );
  thread_2 = _Thread_Get( t2, &lock_context_2 );

  if ( thread_2 != NULL ) {
    _ISR_lock_ISR_enable( &lock_context_2 );
  }

  if ( thread_1 != NULL ) {
    _ISR_lock_ISR_enable( &lock_context_1 );
  }

  return thread_1 != NULL && thread_1 == thread_2;
#endif
}
