/**
 * @file
 *
 * @brief Thread Queue Enqueue Priority
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
#include <rtems/score/statesimpl.h>

Thread_blocking_operation_States _Thread_queue_Enqueue_priority (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_Level            *level_p
)
{
  Thread_blocking_operation_States sync_state;
  ISR_Level                        level;

  _ISR_Disable( level );

    sync_state = the_thread_queue->sync_state;
    the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;
    if (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) {
      _RBTree_Insert(
        &the_thread_queue->Queues.Priority,
        &the_thread->RBNode,
        _Thread_queue_Compare_priority,
        false
      );
      the_thread->Wait.queue = the_thread_queue;
      the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;
      _ISR_Enable( level );
      return THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;
    }

  /*
   *  An interrupt completed the thread's blocking request.
   *  For example, the blocking thread could have been given
   *  the mutex by an ISR or timed out.
   *
   *  WARNING! Returning with interrupts disabled!
   */
  *level_p = level;
  return sync_state;
}
