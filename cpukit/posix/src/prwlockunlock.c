/**
 * @file
 *
 * @brief Function Releases a lock held on RWLock object referenced by rwlock 
 * @ingroup POSIXAPI
 */

/*
 *  POSIX RWLock Manager -- Release a lock held on a RWLock Instance
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

#include <rtems/posix/rwlockimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_rwlock_unlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Thread_queue_Context  queue_context;
  Status_Control        status;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &queue_context );

  if ( the_rwlock == NULL ) {
    return EINVAL;
  }

  status = _CORE_RWLock_Surrender( &the_rwlock->RWLock, &queue_context );
  return _POSIX_Get_error( status );
}
