/**
 * @file
 *
 * @brief Thread queue Extract priority Helper
 * @ingroup ScoreThreadQ
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_queue_Extract_priority_helper(
  Thread_Control       *the_thread,
  uint32_t              return_code,
  bool                  requeuing
)
{
  ISR_Level       level;

  _ISR_Disable( level );
  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return;
  }

  /*
   *  The thread was actually waiting on a thread queue so let's remove it.
   */
  _RBTree_Extract(
    &the_thread->Wait.queue->Queues.Priority,
    &the_thread->RBNode
  );

  /*
   *  If we are not supposed to touch timers or the thread's state, return.
   */
  if ( requeuing ) {
    _ISR_Enable( level );
    return;
  }

  the_thread->Wait.queue = NULL;
  the_thread->Wait.return_code = return_code;

  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }
  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif
}
