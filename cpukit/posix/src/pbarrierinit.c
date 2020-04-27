/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Call to Function Enables Reinitializing of the Barrier
 */

/*
 *  POSIX Barrier Manager -- Initialize a Barrier Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/posixapi.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, flags )
    == offsetof( pthread_barrier_t, _flags ),
  POSIX_BARRIER_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, count )
    == offsetof( pthread_barrier_t, _count ),
  POSIX_BARRIER_CONTROL_COUNT
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, waiting_threads )
    == offsetof( pthread_barrier_t, _waiting_threads ),
  POSIX_BARRIER_CONTROL_WAITING_THREADS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Barrier_Control, Queue )
    == offsetof( pthread_barrier_t, _Queue ),
  POSIX_BARRIER_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_Barrier_Control ) == sizeof( pthread_barrier_t ),
  POSIX_BARRIER_CONTROL_SIZE
);

int pthread_barrier_init(
  pthread_barrier_t           *_barrier,
  const pthread_barrierattr_t *attr,
  unsigned int                 count
)
{
  POSIX_Barrier_Control *barrier;

  barrier = _POSIX_Barrier_Get( _barrier );

  /*
   *  Error check parameters
   */
  if ( barrier == NULL ) {
    return EINVAL;
  }

  if ( count == 0 ) {
    return EINVAL;
  }

  if ( attr != NULL ) {
    /*
     * Now start error checking the attributes that we are going to use
     */
    if ( !attr->is_initialized )
      return EINVAL;

    if ( !_POSIX_Is_valid_pshared( attr->process_shared ) ) {
      return EINVAL;
    }
  }

  barrier->flags = (uintptr_t) barrier ^ POSIX_BARRIER_MAGIC;
  barrier->count = count;
  barrier->waiting_threads = 0;
  _Thread_queue_Queue_initialize( &barrier->Queue.Queue, NULL );
  return 0;
}
