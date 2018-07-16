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

#include <string.h>

bool _POSIX_RWLock_Auto_initialization( POSIX_RWLock_Control *the_rwlock )
{
  POSIX_RWLock_Control zero;

  memset( &zero, 0, sizeof( zero ) );

  if ( memcmp( the_rwlock, &zero, sizeof( *the_rwlock ) ) != 0 ) {
    return false;
  }

  the_rwlock->flags = (uintptr_t) the_rwlock ^ POSIX_RWLOCK_MAGIC;
  return true;
}

int pthread_rwlock_unlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Status_Control        status;

  the_rwlock = _POSIX_RWLock_Get( rwlock );
  POSIX_RWLOCK_VALIDATE_OBJECT( the_rwlock );

  status = _CORE_RWLock_Surrender( &the_rwlock->RWLock );
  return _POSIX_Get_error( status );
}
