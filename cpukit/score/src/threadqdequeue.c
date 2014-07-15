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
      the_thread = _RBTree_Container_of( first, Thread_Control, RBNode );
    }
  }

  /*
   * We did not find a thread to unblock.
   */
  if ( !the_thread ) {
    sync_state = the_thread_queue->sync_state;
    if ( (sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT) ||
         (sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED) ) {
      the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_SATISFIED;
      the_thread = _Thread_Executing;
    }
    _ISR_Enable( level );
    return NULL;
  }

  /*
   * We found a thread to unblock.
   */
  the_thread->Wait.queue = NULL;
  if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
    _ISR_Enable( level );
  } else {
    _Watchdog_Deactivate( &the_thread->Timer );
    _ISR_Enable( level );
    (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) )
    _Thread_MP_Free_proxy( the_thread );
#endif

  return the_thread;
}
