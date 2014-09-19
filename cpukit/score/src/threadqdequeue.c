/**
 * @file
 *
 * @brief Thread Queue Dequeue
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/watchdogimpl.h>

Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control *the_thread;
  ISR_Level       level;
  Thread_blocking_operation_States  sync_state;

  the_thread = NULL;
  _ISR_Disable( level );

  /*
   * Invoke the discipline specific dequeue method.
   */
  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_FIFO ) {
    if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) ) {
      the_thread = (Thread_Control *)
       _Chain_Get_first_unprotected( &the_thread_queue->Queues.Fifo );
    }
  } else { /* must be THREAD_QUEUE_DISCIPLINE_PRIORITY */
    RBTree_Node    *first;

    first = _RBTree_Get( &the_thread_queue->Queues.Priority, RBT_LEFT );
    if ( first ) {
      the_thread = THREAD_RBTREE_NODE_TO_THREAD( first );
    }
  }

  if ( the_thread == NULL ) {
    /*
     * We did not find a thread to unblock in the queue.  Maybe the executing
     * thread is about to block on this thread queue.
     */
    sync_state = the_thread_queue->sync_state;
    if ( (sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT) ||
         (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) ) {
      the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SATISFIED;
      the_thread = _Thread_Executing;
    } else {
      _ISR_Enable( level );
      return NULL;
    }
  }

  /*
   * We found a thread to unblock.
   *
   * NOTE: This is invoked with interrupts still disabled.
   */
  _Thread_blocking_operation_Finalize( the_thread, level );

  return the_thread;
}
