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

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Semaphore_Control, Semaphore.Wait_queue );

int _POSIX_Semaphore_Wait_support(
  sem_t             *sem,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Thread_Control          *executing;
  Thread_queue_Context     queue_context;

  the_semaphore = _POSIX_Semaphore_Get( sem, &queue_context );

  if ( the_semaphore == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  executing = _Thread_Executing;

  _CORE_semaphore_Seize(
    &the_semaphore->Semaphore,
    executing,
    blocking,
    timeout,
    &queue_context
  );

  if ( executing->Wait.return_code == CORE_SEMAPHORE_STATUS_SUCCESSFUL ) {
    return 0;
  }

  rtems_set_errno_and_return_minus_one(
    _POSIX_Semaphore_Translate_core_semaphore_return_code(
      executing->Wait.return_code
    )
  );
}
