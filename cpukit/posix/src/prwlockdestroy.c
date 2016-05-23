/**
 *  @file
 *
 *  @brief Destroy a RWLock
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

#include <rtems/posix/rwlockimpl.h>

int pthread_rwlock_destroy(
  pthread_rwlock_t *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Thread_queue_Context  queue_context;

  _Objects_Allocator_lock();
  the_rwlock = _POSIX_RWLock_Get( rwlock, &queue_context );

  if ( the_rwlock == NULL ) {
    _Objects_Allocator_unlock();
    return EINVAL;
  }

  _CORE_RWLock_Acquire_critical( &the_rwlock->RWLock, &queue_context );

  /*
   *  If there is at least one thread waiting, then do not delete it.
   */

  if ( !_Thread_queue_Is_empty( &the_rwlock->RWLock.Wait_queue.Queue ) ) {
    _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
    _Objects_Allocator_unlock();
    return EBUSY;
  }

  /*
   *  POSIX doesn't require behavior when it is locked.
   */

  _Objects_Close( &_POSIX_RWLock_Information, &the_rwlock->Object );
  _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
  _POSIX_RWLock_Free( the_rwlock );
  _Objects_Allocator_unlock();
  return 0;
}
