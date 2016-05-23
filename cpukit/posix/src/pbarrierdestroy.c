/**
 *  @file
 *
 *  @brief Destroy a Barrier Object 
 *  @ingroup POSIXAPI
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

/**
 *  This directive allows a thread to delete a barrier specified by
 *  the barrier id.  The barrier is freed back to the inactive
 *  barrier chain.
 *
 *  @param[in] barrier is the barrier id
 * 
 *  @return This method returns 0 if there was not an
 *  error. Otherwise, a status code is returned indicating the
 *  source of the error.
 */
int pthread_barrier_destroy(
  pthread_barrier_t *barrier
)
{
  POSIX_Barrier_Control *the_barrier;
  Thread_queue_Context   queue_context;

  if ( barrier == NULL ) {
    return EINVAL;
  }

  _Objects_Allocator_lock();
  the_barrier = _POSIX_Barrier_Get( barrier, &queue_context );

  if ( the_barrier == NULL ) {
    _Objects_Allocator_unlock();
    return EINVAL;
  }

  _CORE_barrier_Acquire_critical( &the_barrier->Barrier, &queue_context );

  if ( the_barrier->Barrier.number_of_waiting_threads != 0 ) {
    _CORE_barrier_Release( &the_barrier->Barrier, &queue_context );
    _Objects_Allocator_unlock();
    return EBUSY;
  }

  _Objects_Close( &_POSIX_Barrier_Information, &the_barrier->Object );
  _CORE_barrier_Release( &the_barrier->Barrier, &queue_context );
  _POSIX_Barrier_Free( the_barrier );
  _Objects_Allocator_unlock();
  return 0;
}
