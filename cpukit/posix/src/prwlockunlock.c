/*
 *  POSIX RWLock Manager -- Release a lock held on a RWLock Instance
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/rwlock.h>

/*
 *  pthread_rwlock_unlock
 *
 *  This directive attempts to release a lock on an RWLock.
 *
 *  Input parameters:
 *    rwlock          - pointer to rwlock id
 *
 *  Output parameters:
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_rwlock_unlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control  *the_rwlock;
  Objects_Locations      location;
  CORE_RWLock_Status     status;

  if ( !rwlock )
    return EINVAL;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      status = _CORE_RWLock_Release( &the_rwlock->RWLock );
      _Thread_Enable_dispatch();
      return _POSIX_RWLock_Translate_core_RWLock_return_code( status );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
