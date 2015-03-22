/**
 * @file
 *
 * @brief Thread Queue Enqueue
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
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_blocking_operation_Finalize(
  Thread_Control                   *the_thread,
  ISR_Level                         level
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
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  } else
    _ISR_Enable( level );

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

/**
 *  @brief Cancel a blocking operation due to ISR.
 *
 *  This method is used to cancel a blocking operation that was
 *  satisfied from an ISR while the thread executing was in the
 *  process of blocking.
 *
 *  This method will restore the previous ISR disable level during the cancel
 *  operation.  Thus it is an implicit _ISR_Enable().
 *
 *  @param[in] sync_state is the synchronization state
 *  @param[in] the_thread is the thread whose blocking is canceled
 *  @param[in] level is the previous ISR disable level
 *
 *  @note This is a rare routine in RTEMS.  It is called with
 *        interrupts disabled and only when an ISR completed
 *        a blocking condition in process.
 */
static void _Thread_blocking_operation_Cancel(
  Thread_blocking_operation_States  sync_state,
  Thread_Control                   *the_thread,
  ISR_Level                         level
)
{
  /*
   *  Cases that should not happen and why.
   *
   *  THREAD_BLOCKING_OPERATION_SYNCHRONIZED:
   *
   *  This indicates that someone did not enter a blocking
   *  operation critical section.
   *
   *  THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED:
   *
   *  This indicates that there was nothing to cancel so
   *  we should not have been called.
   */

  #if defined(RTEMS_DEBUG)
    if ( (sync_state == THREAD_BLOCKING_OPERATION_SYNCHRONIZED) ||
         (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) ) {
      _Terminate(
        INTERNAL_ERROR_CORE,
        true,
        INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL
      );
    }
  #else
    (void) sync_state;
  #endif

  _Thread_blocking_operation_Finalize( the_thread, level );
}

void _Thread_queue_Enqueue_with_handler(
  Thread_queue_Control         *the_thread_queue,
  Thread_Control               *the_thread,
  Watchdog_Interval             timeout,
  Thread_queue_Timeout_callout  handler
)
{
  ISR_Level                         level;
  Thread_blocking_operation_States  sync_state;

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
       handler,
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
  _ISR_Disable( level );

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
      _RBTree_Insert(
        &the_thread_queue->Queues.Priority,
        &the_thread->RBNode,
        _Thread_queue_Compare_priority,
        false
      );
    }

    the_thread->Wait.queue = the_thread_queue;
    the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;
    _ISR_Enable( level );
    return;
  } else { /* sync_state != THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) */
    _Thread_blocking_operation_Cancel( sync_state, the_thread, level );
  }
}
