/**
 * @file
 *
 * @brief Obtain a Read Lock on a RWLock Instance
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
#include <rtems/posix/posixapi.h>

int pthread_rwlock_rdlock(
  pthread_rwlock_t  *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock;
  Thread_queue_Context  queue_context;
  Status_Control        status;

  the_rwlock = _POSIX_RWLock_Get( rwlock );
  POSIX_RWLOCK_VALIDATE_OBJECT( the_rwlock );

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
  status = _CORE_RWLock_Seize_for_reading(
    &the_rwlock->RWLock,
    true,                 /* we are willing to wait forever */
    &queue_context
  );
  return _POSIX_Get_error( status );
}
