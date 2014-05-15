/**
 * @file
 *
 * @brief Changes the Priority of a Thread
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

/**
 *  @brief Invoked when a thread changes priority and is blocked.
 *
 *  This routine is invoked when a thread changes priority and is
 *  blocked on a thread queue.  If the queue is priority ordered,
 *  the_thread is removed from the_thread_queue and reinserted using
 *  its new priority.  This method has no impact on the state of the_thread
 *  or of any timeouts associated with this blocking.
 *
 *  @param[in] the_thread_queue pointer to a threadq header
 *  @param[in] the_thread pointer to a thread control block
 */
static void _Thread_queue_Requeue(
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
      _Thread_queue_Extract_priority_helper(
        the_thread,
        the_thread->Wait.return_code,
        true
      );
      (void) _Thread_queue_Enqueue_priority( tq, the_thread, &level_ignored );
    }
    _ISR_Enable( level );
  }
}

void _Thread_Change_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
  /*
   *  Do not bother recomputing all the priority related information if
   *  we are not REALLY changing priority.
   */
  if ( the_thread->current_priority != new_priority ) {
    ISR_Level                level;
    const Scheduler_Control *scheduler;

    _ISR_Disable( level );

    scheduler = _Scheduler_Get( the_thread );
    the_thread->current_priority = new_priority;

    if ( _States_Is_ready( the_thread->current_state ) ) {
      _Scheduler_Change_priority(
        scheduler,
        the_thread,
        new_priority,
        prepend_it
      );

      _ISR_Flash( level );

      /*
       *  We altered the set of thread priorities.  So let's figure out
       *  who is the heir and if we need to switch to them.
       */
      scheduler = _Scheduler_Get( the_thread );
      _Scheduler_Schedule( scheduler, the_thread );
    } else {
      _Scheduler_Update( scheduler, the_thread );
    }
    _ISR_Enable( level );

    _Thread_queue_Requeue( the_thread->Wait.queue, the_thread );
  }
}
