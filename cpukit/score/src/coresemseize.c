/*
 *  CORE Semaphore Handler
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

#if defined(RTEMS_SCORE_CORESEM_ENABLE_SEIZE_BODY)
/*
 *  This routine attempts to allocate a core semaphore to the calling thread.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *    id            - id of object to wait on
 *    wait          - true if wait is allowed, false otherwise
 *    timeout       - number of ticks to wait (0 means forever)
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_semaphore_Seize(
  CORE_semaphore_Control *the_semaphore,
  Objects_Id              id,
  bool                    wait,
  Watchdog_Interval       timeout
)
{
  Thread_Control *executing;
  ISR_Level       level;

  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _ISR_Enable( level );
    return;
  }

  /*
   *  If the semaphore was not available and the caller was not willing
   *  to block, then return immediately with a status indicating that
   *  the semaphore was not available and the caller never blocked.
   */
  if ( !wait ) {
    _ISR_Enable( level );
    executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
    return;
  }

  /*
   *  If the semaphore is not available and the caller is willing to
   *  block, then we now block the caller with optional timeout.
   */
  _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
  executing->Wait.queue = &the_semaphore->Wait_queue;
  executing->Wait.id    = id;
  _ISR_Enable( level );
  _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
}
#endif
