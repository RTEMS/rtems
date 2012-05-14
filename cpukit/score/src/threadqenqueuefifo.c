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
 *  _Thread_queue_Enqueue_fifo
 *
 *  This routine places a blocked thread on a FIFO thread queue.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *    the_thread       - pointer to the thread to block
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

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
