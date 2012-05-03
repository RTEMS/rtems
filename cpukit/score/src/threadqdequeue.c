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
 *  _Thread_queue_Dequeue
 *
 *  This routine removes a thread from the specified threadq.  If the
 *  threadq discipline is FIFO, it unblocks a thread, and cancels its
 *  timeout timer.  Priority discipline is processed elsewhere.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to threadq
 *
 *  Output parameters:
 *    returns - thread dequeued or NULL
 *
 *  INTERRUPT LATENCY:
 *    check sync
 */

Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control *(*dequeue_p)( Thread_queue_Control * );
  Thread_Control *the_thread;
  ISR_Level       level;
  Thread_blocking_operation_States  sync_state;

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
    dequeue_p = _Thread_queue_Dequeue_priority;
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    dequeue_p = _Thread_queue_Dequeue_fifo;

  the_thread = (*dequeue_p)( the_thread_queue );
  _ISR_Disable( level );
    if ( !the_thread ) {
      sync_state = the_thread_queue->sync_state;
      if ( (sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT) ||
           (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) ) {
        the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SATISFIED;
        the_thread = _Thread_Executing;
      }
    }
  _ISR_Enable( level );
  return the_thread;
}
