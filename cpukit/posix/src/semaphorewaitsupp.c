/**
 * @file
 *
 * @brief POSIX Semaphore Wait Support
 * @ingroup POSIXSemaphorePrivate
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <semaphore.h>

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/posixapi.h>

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Semaphore_Control, Semaphore.Wait_queue );

int _POSIX_Semaphore_Wait_support(
  sem_t             *sem,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Thread_queue_Context     queue_context;
  Status_Control           status;

  the_semaphore = _POSIX_Semaphore_Get( sem, &queue_context );

  if ( the_semaphore == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  status = _CORE_semaphore_Seize(
    &the_semaphore->Semaphore,
    POSIX_SEMAPHORE_TQ_OPERATIONS,
    _Thread_Executing,
    blocking,
    timeout,
    &queue_context
  );
  return _POSIX_Zero_or_minus_one_plus_errno( status );
}
