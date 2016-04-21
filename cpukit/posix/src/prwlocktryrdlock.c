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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/rwlockimpl.h>

int pthread_rwlock_tryrdlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  ISR_lock_Context      lock_context;
  Thread_Control       *executing;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &lock_context );

  if ( the_rwlock == NULL ) {
    return EINVAL;
  }

  executing = _Thread_Executing;
  _CORE_RWLock_Seize_for_reading(
    &the_rwlock->RWLock,
    executing,
    false,                  /* do not wait for the rwlock */
    0,
    &lock_context
  );
  return _POSIX_RWLock_Translate_core_RWLock_return_code(
    (CORE_RWLock_Status) executing->Wait.return_code
  );
}
