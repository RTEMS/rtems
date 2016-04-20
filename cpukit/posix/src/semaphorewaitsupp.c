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
#include <rtems/score/threadimpl.h>

THREAD_WAIT_QUEUE_OBJECT_ASSERT(
  POSIX_Semaphore_Control,
  Semaphore.Wait_queue
);

int _POSIX_Semaphore_Wait_support(
  sem_t             *sem,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Thread_Control          *executing;
  ISR_lock_Context         lock_context;

  the_semaphore = _POSIX_Semaphore_Get( sem, &lock_context );

  if ( the_semaphore == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  executing = _Thread_Executing;

  _CORE_semaphore_Seize(
    &the_semaphore->Semaphore,
    executing,
    the_semaphore->Object.id,
    blocking,
    timeout,
    &lock_context
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
