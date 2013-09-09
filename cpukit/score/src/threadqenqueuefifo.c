/**
 *  @file
 *
 *  @brief  Thread queue Enqueue FIFO
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
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlevel.h>

Thread_blocking_operation_States _Thread_queue_Enqueue_fifo (
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
      _Chain_Append_unprotected(
        &the_thread_queue->Queues.Fifo,
        &the_thread->Object.Node
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
