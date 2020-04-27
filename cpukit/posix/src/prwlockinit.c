/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Allocate resources to use the read-write lock and Initialize it
 */

/*
 *  POSIX RWLock Manager -- Destroy a RWLock Instance
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/rwlockimpl.h>
#include <rtems/posix/posixapi.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, flags )
    == offsetof( pthread_rwlock_t, _flags ),
  POSIX_RWLOCK_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.current_state )
    == offsetof( pthread_rwlock_t, _current_state ),
  POSIX_RWLOCK_CONTROL_CURRENT_STATE
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.number_of_readers )
    == offsetof( pthread_rwlock_t, _number_of_readers ),
  POSIX_RWLOCK_CONTROL_NUMBER_OF_READERS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_RWLock_Control, RWLock.Queue )
    == offsetof( pthread_rwlock_t, _Queue ),
  POSIX_RWLOCK_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_RWLock_Control ) == sizeof( pthread_rwlock_t ),
  POSIX_RWLOCK_CONTROL_SIZE
);

int pthread_rwlock_init(
  pthread_rwlock_t           *rwlock,
  const pthread_rwlockattr_t *attr
)
{
  POSIX_RWLock_Control *the_rwlock;

  the_rwlock = _POSIX_RWLock_Get( rwlock );

  if ( the_rwlock == NULL ) {
    return EINVAL;
  }

  if ( attr != NULL ) {
    if ( !attr->is_initialized ) {
      return EINVAL;
    }

    if ( !_POSIX_Is_valid_pshared( attr->process_shared ) ) {
      return EINVAL;
    }
  }

  the_rwlock->flags = (uintptr_t) the_rwlock ^ POSIX_RWLOCK_MAGIC;
  _CORE_RWLock_Initialize( &the_rwlock->RWLock );
  return 0;
}
