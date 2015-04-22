/**
 * @file
 *
 * @brief Thread Queue Operations
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
#include <rtems/score/assert.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

ISR_LOCK_DEFINE( static, _Thread_queue_Lock, "Thread Queue" )

static void _Thread_queue_Acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire( &_Thread_queue_Lock, lock_context );
}

static void _Thread_queue_Release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable( &_Thread_queue_Lock, lock_context );
}

/**
 *  @brief Finalize a blocking operation.
 *
 *  This method is used to finalize a blocking operation that was
 *  satisfied. It may be used with thread queues or any other synchronization
 *  object that uses the blocking states and watchdog times for timeout.
 *
 *  This method will restore the previous ISR disable level during the cancel
 *  operation.  Thus it is an implicit _ISR_Enable().
 *
 *  @param[in] the_thread is the thread whose blocking is canceled
 *  @param[in] lock_context is the previous ISR disable level
 */
static void _Thread_blocking_operation_Finalize(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  /*
   * The thread is not waiting on anything after this completes.
   */
  the_thread->Wait.queue = NULL;

  /*
   *  If the sync state is timed out, this is very likely not needed.
   *  But better safe than sorry when it comes to critical sections.
   */
  if ( _Watchdog_Is_active( &the_thread->Timer ) ) {
    _Watchdog_Deactivate( &the_thread->Timer );
    _Thread_queue_Release( lock_context );
    (void) _Watchdog_Remove( &the_thread->Timer );
  } else
    _Thread_queue_Release( lock_context );

  /*
   *  Global objects with thread queue's should not be operated on from an
   *  ISR.  But the sync code still must allow short timeouts to be processed
   *  correctly.
   */

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif
}

static void _Thread_queue_Requeue_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  void             *context
)
{
  Thread_queue_Control *tq = context;

  _RBTree_Extract( &tq->Queues.Priority, &the_thread->RBNode );
  _RBTree_Insert(
    &tq->Queues.Priority,
    &the_thread->RBNode,
    _Thread_queue_Compare_priority,
    false
  );
}

void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  Watchdog_Interval     timeout
)
{
  ISR_lock_Context                 lock_context;
  Thread_blocking_operation_States sync_state;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet )
    the_thread = _Thread_MP_Allocate_proxy( the_thread_queue->state );
  else
#endif
  /*
   *  Set the blocking state for this thread queue in the thread.
   */
  _Thread_Set_state( the_thread, the_thread_queue->state );

  /*
   *  If the thread wants to timeout, then schedule its timer.
   */
  if ( timeout ) {
    _Watchdog_Initialize(
       &the_thread->Timer,
       _Thread_queue_Timeout,
       the_thread->Object.id,
       NULL
    );

    _Watchdog_Insert_ticks( &the_thread->Timer, timeout );
  }

  /*
   * Now initiate the enqueuing and checking if the blocking operation
   * should be completed or the thread has had its blocking condition
   * satisfied before we got here.
   */
  _Thread_queue_Acquire( &lock_context );

  sync_state = the_thread_queue->sync_state;
  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) {
    /*
     * Invoke the discipline specific enqueue method.
     */
    if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_FIFO ) {
      _Chain_Append_unprotected(
        &the_thread_queue->Queues.Fifo,
        &the_thread->Object.Node
      );
    } else { /* must be THREAD_QUEUE_DISCIPLINE_PRIORITY */
      _Thread_Lock_set( the_thread, &_Thread_queue_Lock );
      _Thread_Priority_set_change_handler(
        the_thread,
        _Thread_queue_Requeue_priority,
        the_thread_queue
      );
      _RBTree_Insert(
        &the_thread_queue->Queues.Priority,
        &the_thread->RBNode,
        _Thread_queue_Compare_priority,
        false
      );
    }

    the_thread->Wait.queue = the_thread_queue;
    the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;
    _Thread_queue_Release( &lock_context );
  } else {
    /* Cancel a blocking operation due to ISR */

    _Assert(
      sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT ||
        sync_state == THREAD_BLOCKING_OPERATION_SATISFIED
    );

    _Thread_blocking_operation_Finalize( the_thread, &lock_context );
  }
}

void _Thread_queue_Extract_with_return_code(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  uint32_t              return_code
)
{
  ISR_lock_Context lock_context;

  _Thread_queue_Acquire( &lock_context );

  if ( !_States_Is_waiting_on_thread_queue( the_thread->current_state ) ) {
    _Thread_queue_Release( &lock_context );
    return;
  }

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_FIFO ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
  } else { /* must be THREAD_QUEUE_DISCIPLINE_PRIORITY */
    _RBTree_Extract(
      &the_thread->Wait.queue->Queues.Priority,
      &the_thread->RBNode
    );
    _Thread_Priority_restore_default_change_handler( the_thread );
    _Thread_Lock_restore_default( the_thread );
  }

  the_thread->Wait.return_code = return_code;

  /*
   * We found a thread to unblock.
   *
   * NOTE: This is invoked with interrupts still disabled.
   */
  _Thread_blocking_operation_Finalize( the_thread, &lock_context );
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

Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control                   *the_thread;
  ISR_lock_Context                  lock_context;
  Thread_blocking_operation_States  sync_state;

  the_thread = NULL;
  _Thread_queue_Acquire( &lock_context );

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
      _Thread_Priority_restore_default_change_handler( the_thread );
      _Thread_Lock_restore_default( the_thread );
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
      _Thread_queue_Release( &lock_context );
      return NULL;
    }
  }

  /*
   * We found a thread to unblock.
   *
   * NOTE: This is invoked with interrupts still disabled.
   */
  _Thread_blocking_operation_Finalize( the_thread, &lock_context );

  return the_thread;
}
