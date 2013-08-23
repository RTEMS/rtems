/**
 * @file
 * 
 * @brief Thread Queue Handler Process Timeout Handler
 * @ingroup ScoreThreadQ
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
#include <rtems/score/threadimpl.h>

void _Thread_queue_Process_timeout(
  Thread_Control *the_thread
)
{
  Thread_queue_Control *the_thread_queue;
  ISR_Level             level;

  /*
   *  If the_thread_queue is not synchronized, then it is either
   *  "nothing happened", "timeout", or "satisfied".   If the_thread
   *  is the executing thread, then it is in the process of blocking
   *  and it is the thread which is responsible for the synchronization
   *  process.
   *
   *  If it is not satisfied, then it is "nothing happened" and
   *  this is the "timeout" transition.  After a request is satisfied,
   *  a timeout is not allowed to occur.
   */

  _ISR_Disable( level );
  the_thread_queue = the_thread->Wait.queue;
  if ( the_thread_queue != NULL ) {
    if ( the_thread_queue->sync_state != THREAD_BLOCKING_OPERATION_SYNCHRONIZED &&
         _Thread_Is_executing( the_thread ) ) {
      if ( the_thread_queue->sync_state != THREAD_BLOCKING_OPERATION_SATISFIED ) {
        the_thread->Wait.return_code = the_thread_queue->timeout_status;
        the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_TIMEOUT;
      }
      _ISR_Enable( level );
    } else {
      bool we_did_it;

      _ISR_Enable( level );

      /*
       * After we enable interrupts here, a lot may happen in the meantime,
       * e.g. nested interrupts may release the resource that times out here.
       * So we enter _Thread_queue_Extract() speculatively.  Inside this
       * function we check the actual status under ISR disable protection.
       * This ensures that exactly one executing context performs the extract
       * operation (other parties may call _Thread_queue_Dequeue()).  If this
       * context won, then we have a timeout.
       *
       * We can use the_thread_queue pointer here even if
       * the_thread->Wait.queue is already set to NULL since the extract
       * operation will only use the thread queue discipline to select the
       * right extract operation.  The timeout status is set during thread
       * queue initialization.
       */
      we_did_it = _Thread_queue_Extract( the_thread_queue, the_thread );
      if ( we_did_it ) {
        the_thread->Wait.return_code = the_thread_queue->timeout_status;
      }
    }
  } else {
    _ISR_Enable( level );
  }
}

