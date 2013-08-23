/**
 *  @file
 *
 *  @brief Thread Queue Requeue
 *  @ingroup ScoreThreadQ
 */

/*
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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>

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
    ISR_Level             level_ignored;

    _ISR_Disable( level );
    if ( _States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
      _Thread_queue_Enter_critical_section( tq );
      _Thread_queue_Extract_priority_helper( the_thread, true );
      (void) _Thread_queue_Enqueue_priority( tq, the_thread, &level_ignored );
    }
    _ISR_Enable( level );
  }
}

