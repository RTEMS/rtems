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
  pthread_rwlock_t *_rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Thread_queue_Context  queue_context;

  the_rwlock = _POSIX_RWLock_Get( _rwlock );
  POSIX_RWLOCK_VALIDATE_OBJECT( the_rwlock );

  _CORE_RWLock_Acquire( &the_rwlock->RWLock, &queue_context );

  /*
   *  If there is at least one thread waiting, then do not delete it.
   */

  if ( !_Thread_queue_Is_empty( &the_rwlock->RWLock.Queue.Queue ) ) {
    _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
    return EBUSY;
  }

  /*
   *  POSIX doesn't require behavior when it is locked.
   */

  the_rwlock->flags = ~the_rwlock->flags;
  _CORE_RWLock_Release( &the_rwlock->RWLock, &queue_context );
  return 0;
}
