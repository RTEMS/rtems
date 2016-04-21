/**
 * @file
 *
 * @brief Obtain a Write Lock on a RWlock Instance
 * @ingroup POSIXAPI
 */

/*
 *  POSIX RWLock Manager -- Obtain a Write Lock on a RWLock Instance
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

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/rwlockimpl.h>
#include <rtems/score/threadimpl.h>

THREAD_WAIT_QUEUE_OBJECT_ASSERT( POSIX_RWLock_Control, RWLock.Wait_queue );

int pthread_rwlock_wrlock(
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
  _CORE_RWLock_Seize_for_writing(
    &the_rwlock->RWLock,
    executing,
    true,          /* do not timeout -- wait forever */
    0,
    &lock_context
  );
  return _POSIX_RWLock_Translate_core_RWLock_return_code(
    (CORE_RWLock_Status) executing->Wait.return_code
  );
}
