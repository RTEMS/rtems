/*
 *  SuperCore Barrier Handler
 *
 *  DESCRIPTION:
 *
 *  This package is part of the implementation of the SuperCore Barrier Handler.
 *
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/score/corebarrier.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_barrier_Wait
 *
 *  Input parameters:
 *    the_barrier - pointer to barrier control block
 *    id          - id of object to wait on
 *    wait        - true if wait is allowed, false otherwise
 *    timeout     - number of ticks to wait (0 means forever)
 *    api_barrier_mp_support - api dependent MP support actions
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

void _CORE_barrier_Wait(
  CORE_barrier_Control                *the_barrier,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
)
{
  Thread_Control *executing;
  ISR_Level       level;

  executing = _Thread_Executing;
  executing->Wait.return_code = CORE_BARRIER_STATUS_SUCCESSFUL;
  _ISR_Disable( level );
  the_barrier->number_of_waiting_threads++;
  if ( _CORE_barrier_Is_automatic( &the_barrier->Attributes ) ) {
    if ( the_barrier->number_of_waiting_threads ==
	 the_barrier->Attributes.maximum_count) {
      executing->Wait.return_code = CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED;
      _ISR_Enable( level );
      _CORE_barrier_Release( the_barrier, id, api_barrier_mp_support );
      return;
    }
  }

  _Thread_queue_Enter_critical_section( &the_barrier->Wait_queue );
  executing->Wait.queue          = &the_barrier->Wait_queue;
  executing->Wait.id             = id;
  _ISR_Enable( level );

  _Thread_queue_Enqueue( &the_barrier->Wait_queue, timeout );
}
