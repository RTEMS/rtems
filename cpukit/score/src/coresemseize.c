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
 *
 *  $Id$
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
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

/*PAGE
 *
 *  _CORE_semaphore_Seize
 *
 *  This routine attempts to allocate a core semaphore to the calling thread.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *    id            - id of object to wait on
 *    wait          - TRUE if wait is allowed, FALSE otherwise
 *    timeout       - number of ticks to wait (0 means forever)
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_semaphore_Seize(
  CORE_semaphore_Control         *the_semaphore,
  Objects_Id                      id,
  Core_semaphore_Blocking_option  wait,
  Watchdog_Interval               timeout
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
  if ( wait == CORE_SEMAPHORE_NO_WAIT ) {
      _ISR_Enable( level );
      executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;
      return;
  }

  /*
   *  This is strange case because normally RTEMS validates parameters
   *  before performing any real work.  But in this case, the POSIX
   *  API requires that a semaphore be checked for immediate availability
   *  BEFORE the timeout value is validated.  This is implemented in
   *  RTEMS by indicating a special status that the timeout value
   *  was invalid which is returned in this case.
   */
  if ( wait == CORE_SEMAPHORE_BAD_TIMEOUT ) {
      _ISR_Enable( level );
      executing->Wait.return_code = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
      return;
  }

  /*
   *  If the semaphore is not available and the caller is willing to
   *  block, then we now block the caller with optional timeout.
   */
  if (( wait == CORE_SEMAPHORE_BLOCK_FOREVER) ||
      ( wait == CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT ) ) {
      _Thread_queue_Enter_critical_section( &the_semaphore->Wait_queue );
      executing->Wait.queue          = &the_semaphore->Wait_queue;
      executing->Wait.id             = id;
      _ISR_Enable( level );
      _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
  }
}
