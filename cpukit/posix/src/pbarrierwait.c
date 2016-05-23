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

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Barrier_Control, Barrier.Wait_queue );

/**
 * This directive allows a thread to wait at a barrier.
 *
 * @param[in] barrier is the barrier id
 *
 * @retval 0 if successful
 * @retval PTHREAD_BARRIER_SERIAL_THREAD if successful
 * @retval error_code if unsuccessful
 */

int pthread_barrier_wait(
  pthread_barrier_t *barrier
)
{
  POSIX_Barrier_Control *the_barrier;
  Thread_queue_Context   queue_context;
  Thread_Control        *executing;

  if ( barrier == NULL ) {
    return EINVAL;
  }

  the_barrier = _POSIX_Barrier_Get( barrier, &queue_context );

  if ( the_barrier == NULL ) {
    return EINVAL;
  }

  executing = _Thread_Executing;
  _CORE_barrier_Seize(
    &the_barrier->Barrier,
    executing,
    true,
    0,
    &queue_context
  );
  return _POSIX_Barrier_Translate_core_barrier_return_code(
    executing->Wait.return_code
  );
}
