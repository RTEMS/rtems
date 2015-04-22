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
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

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
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_lock_Context     *lock_context
)
{
  /*
   * The thread is not waiting on anything after this completes.
   */
  _Thread_Wait_set_queue( the_thread, NULL );
  _Thread_Wait_restore_default_operations( the_thread );

  _Thread_Lock_restore_default( the_thread );

  /*
   *  If the sync state is timed out, this is very likely not needed.
   *  But better safe than sorry when it comes to critical sections.
   */
  if ( _Watchdog_Is_active( &the_thread->Timer ) ) {
    _Watchdog_Deactivate( &the_thread->Timer );
    _Thread_queue_Release( the_thread_queue, lock_context );
    _Watchdog_Remove_ticks( &the_thread->Timer );
  } else
    _Thread_queue_Release( the_thread_queue, lock_context );

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

void _Thread_queue_Enqueue_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  States_Control        state,
  Watchdog_Interval     timeout,
  ISR_lock_Context     *lock_context
)
{
  Thread_blocking_operation_States sync_state;

  _Thread_Lock_set( the_thread, &the_thread_queue->Lock );

  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;
  _Thread_Wait_set_queue( the_thread, the_thread_queue );

  _Thread_queue_Release( the_thread_queue, lock_context );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet )
    the_thread = _Thread_MP_Allocate_proxy( state );
  else
#endif
  /*
   *  Set the blocking state for this thread queue in the thread.
   */
  _Thread_Set_state( the_thread, state );

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
  _Thread_queue_Acquire( the_thread_queue, lock_context );

  sync_state = the_thread_queue->sync_state;
  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) {
    const Thread_queue_Operations *operations;

    the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

    operations = the_thread_queue->operations;
    _Thread_Wait_set_operations( the_thread, operations );
    ( *operations->enqueue )( the_thread_queue, the_thread );

    _Thread_queue_Release( the_thread_queue, lock_context );
  } else {
    /* Cancel a blocking operation due to ISR */

    _Assert(
      sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT ||
        sync_state == THREAD_BLOCKING_OPERATION_SATISFIED
    );

    _Thread_blocking_operation_Finalize( the_thread_queue, the_thread, lock_context );
  }
}

void _Thread_queue_Extract_with_return_code(
  Thread_Control *the_thread,
  uint32_t        return_code
)
{
  Thread_queue_Control *the_thread_queue;
  ISR_lock_Control     *lock;
  ISR_lock_Context      lock_context;

  lock = _Thread_Lock_acquire( the_thread, &lock_context );

  the_thread_queue = the_thread->Wait.queue;
  if ( the_thread_queue == NULL ) {
    _Thread_Lock_release( lock, &lock_context );
    return;
  }

  _SMP_Assert( lock == &the_thread_queue->Lock );

  ( *the_thread_queue->operations->extract )( the_thread_queue, the_thread );

  the_thread->Wait.return_code = return_code;

  /*
   * We found a thread to unblock.
   *
   * NOTE: This is invoked with interrupts still disabled.
   */
  _Thread_blocking_operation_Finalize( the_thread_queue, the_thread, &lock_context );
}

void _Thread_queue_Extract( Thread_Control *the_thread )
{
  _Thread_queue_Extract_with_return_code(
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

  _Thread_queue_Acquire( the_thread_queue, &lock_context );

  the_thread = ( *the_thread_queue->operations->dequeue )( the_thread_queue );
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
      _Thread_queue_Release( the_thread_queue, &lock_context );
      return NULL;
    }
  }

  /*
   * We found a thread to unblock.
   *
   * NOTE: This is invoked with interrupts still disabled.
   */
  _Thread_blocking_operation_Finalize( the_thread_queue, the_thread, &lock_context );

  return the_thread;
}
