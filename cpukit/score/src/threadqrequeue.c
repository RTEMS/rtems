/**
 * @file
 *
 * @brief Thread Queue Requeue
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_queue_Requeue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  /*
   * Just in case the thread really wasn't blocked on a thread queue
   * when we get here.
   */
  if ( !the_thread_queue )
    return;

  /*
   * If queueing by FIFO, there is nothing to do. This only applies to
   * priority blocking discipline.
   */
  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY ) {
    Thread_queue_Control *tq = the_thread_queue;
    ISR_Level             level;

    _ISR_Disable( level );
    if ( _States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
      _Thread_queue_Enter_critical_section( tq );

      /* extract the thread */
      _RBTree_Extract(
        &the_thread->Wait.queue->Queues.Priority,
        &the_thread->RBNode
      );

      /* enqueue the thread at the new priority */
      _RBTree_Insert(
        &the_thread_queue->Queues.Priority,
        &the_thread->RBNode,
        _Thread_queue_Compare_priority,
        false
      );
    }
    _ISR_Enable( level );
  }
}

