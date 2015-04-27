/**
 * @file
 *
 * @brief Core Semaphore Seize
 *
 * @ingroup ScoreSemaphore
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/thread.h>

#if defined(RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY)

void _CORE_semaphore_Seize(
  CORE_semaphore_Control *the_semaphore,
  Thread_Control         *executing,
  Objects_Id              id,
  bool                    wait,
  Watchdog_Interval       timeout
)
{
  ISR_lock_Context lock_context;

  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  _Thread_queue_Acquire( &the_semaphore->Wait_queue, &lock_context );
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _Thread_queue_Release( &the_semaphore->Wait_queue, &lock_context );
    return;
  }

  /*
   *  If the semaphore was not available and the caller was not willing
   *  to block, then return immediately with a status indicating that
   *  the semaphore was not available and the caller never blocked.
   */
  if ( !wait ) {
    _Thread_queue_Release( &the_semaphore->Wait_queue, &lock_context );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  /*
   *  If the semaphore is not available and the caller is willing to
   *  block, then we now block the caller with optional timeout.
   */
  executing->Wait.id = id;
  _Thread_queue_Enqueue_critical(
    &the_semaphore->Wait_queue,
    executing,
    STATES_WAITING_FOR_SEMAPHORE,
    timeout,
    CORE_SEMAPHORE_TIMEOUT,
    &lock_context
  );
}
#endif
