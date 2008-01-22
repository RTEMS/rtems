/** 
 *  @file  rtems/score/threadq.inl
 *
 *  This inline file contains all of the inlined routines associated with
 *  the manipulation of thread queues.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_THREADQ_INL
#define _RTEMS_SCORE_THREADQ_INL

#include <rtems/score/thread.h>

/**
 *  @addtogroup ScoreThreadQ
 * @{
 */

/**
 *
 * @brief Process Thread Queue Timeout
 *
 * This is a shared helper routine which makes it easier to have multiple 
 * object class specific timeout routines.
 *
 * @param[in] the_thread is the thread to extract
 *
 * @note Assume Dispatching is disabled.
 */
static inline void _Thread_queue_Process_timeout(
  Thread_Control *the_thread
)
{
  Thread_queue_Control *the_thread_queue = the_thread->Wait.queue;

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

  if ( the_thread_queue->sync_state != THREAD_BLOCKING_OPERATION_SYNCHRONIZED &&
       _Thread_Is_executing( the_thread ) ) {
    if ( the_thread_queue->sync_state != THREAD_BLOCKING_OPERATION_SATISFIED ) {
      the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
      the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_TIMEOUT;
    }
  } else {
    the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;
    _Thread_queue_Extract( the_thread->Wait.queue, the_thread );
  }
}

/**@}*/

#endif
/* end of include file */
