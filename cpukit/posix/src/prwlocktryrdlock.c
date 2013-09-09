/**
 * @file
 *
 * @brief Attempt to Obtain a Read Lock on a RWLock Instance
 * @ingroup POSIXAPI
 */

/*
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

#include <rtems/posix/rwlockimpl.h>
#include <rtems/score/thread.h>

/*
 *  pthread_rwlock_tryrdlock
 *
 *  This directive attempts to obtain a read only lock on an rwlock instance.
 *
 *  Input parameters:
 *    rwlock          - pointer to rwlock id
 *
 *  Output parameters:
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_rwlock_tryrdlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control  *the_rwlock;
  Objects_Locations      location;
  Thread_Control        *executing;

  if ( !rwlock )
    return EINVAL;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      executing = _Thread_Executing;
      _CORE_RWLock_Obtain_for_reading(
        &the_rwlock->RWLock,
        executing,
        *rwlock,
        false,                  /* do not wait for the rwlock */
        0,
        NULL
      );


      _Objects_Put( &the_rwlock->Object );
      return _POSIX_RWLock_Translate_core_RWLock_return_code(
        (CORE_RWLock_Status) executing->Wait.return_code
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
