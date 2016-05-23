/**
 * @file
 *
 * @brief Wait at a Barrier
 * @ingroup POSIXAPI
 */

/*
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

#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/posixapi.h>

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Barrier_Control, Barrier.Wait_queue );

int pthread_barrier_wait(
  pthread_barrier_t *barrier
)
{
  POSIX_Barrier_Control *the_barrier;
  Thread_queue_Context   queue_context;
  Status_Control         status;

  if ( barrier == NULL ) {
    return EINVAL;
  }

  the_barrier = _POSIX_Barrier_Get( barrier, &queue_context );

  if ( the_barrier == NULL ) {
    return EINVAL;
  }

  status = _CORE_barrier_Seize(
    &the_barrier->Barrier,
    _Thread_Executing,
    true,
    WATCHDOG_NO_TIMEOUT,
    &queue_context
  );
  return _POSIX_Get_error( status );
}
