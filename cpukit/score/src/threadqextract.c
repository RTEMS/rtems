/**
 * @file
 *
 * @brief Extracts Thread from Thread Queue
 *
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

#include <rtems/score/chainimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_queue_Extract_with_return_code(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  uint32_t              return_code
)
{
  ISR_Level level;

  _ISR_Disable( level );

  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return;
  }

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_FIFO ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
  } else { /* must be THREAD_QUEUE_DISCIPLINE_PRIORITY */
    _RBTree_Extract(
      &the_thread->Wait.queue->Queues.Priority,
      &the_thread->RBNode
    );
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

void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _Thread_queue_Extract_with_return_code(
    the_thread_queue,
    the_thread,
    the_thread->Wait.return_code
  );
}
