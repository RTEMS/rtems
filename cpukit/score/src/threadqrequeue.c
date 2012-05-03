/*
 *  Thread Queue Handler
 *
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  _Thread_queue_Requeue
 *
 *  This routine is invoked when a thread changes priority and should be
 *  moved to a different position on the thread queue.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to a threadq header
 *    the_thread       - pointer to a thread control block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY: NONE
 */

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
      _Thread_queue_Extract_priority_helper( tq, the_thread, true );
      (void) _Thread_queue_Enqueue_priority( tq, the_thread, &level_ignored );
    }
    _ISR_Enable( level );
  }
}

