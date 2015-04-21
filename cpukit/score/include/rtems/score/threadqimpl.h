/**
 *  @file  rtems/score/threadq.h
 *
 *  Constants and Structures Associated with the Manipulation of Objects
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of objects.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADQIMPL_H
#define _RTEMS_SCORE_THREADQIMPL_H

#include <rtems/score/threadq.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreThreadQueue
 */
/**@{*/

RTEMS_INLINE_ROUTINE void _Thread_queue_Acquire_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Acquire( &the_thread_queue->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Acquire(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _Thread_queue_Acquire_critical( the_thread_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Release(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_thread_queue->Lock, lock_context );
}

/**
 *  The following type defines the callout used when a remote task
 *  is extracted from a local thread queue.
 */
typedef void ( *Thread_queue_Flush_callout )(
                  Thread_Control *
             );

/**
 *  @brief Gets a pointer to a thread waiting on the_thread_queue.
 *
 *  This function returns a pointer to a thread waiting on
 *  the_thread_queue.  The selection of this thread is based on
 *  the discipline of the_thread_queue.  If no threads are waiting
 *  on the_thread_queue, then NULL is returned.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
);

/**
 * @brief Blocks a thread and places it on a thread queue.
 *
 * This routine blocks a thread, places it on a thread queue, and optionally
 * starts a watchdog in case the timeout interval is not WATCHDOG_NO_TIMEOUT.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock and register it as the new thread lock.
 *
 * @param[in] the_thread_queue The thread queue.
 * @param[in] the_thread The thread to enqueue.
 * @param[in] state The new state of the thread.
 * @param[in] timeout Interval to wait.  Use WATCHDOG_NO_TIMEOUT to block
 * potentially forever.
 * @param[in] lock_context The lock context of the lock acquire.
 */
void _Thread_queue_Enqueue_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  States_Control        state,
  Watchdog_Interval     timeout,
  ISR_lock_Context     *lock_context
);

RTEMS_INLINE_ROUTINE void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  States_Control        state,
  Watchdog_Interval     timeout
)
{
  ISR_lock_Context lock_context;

  _Thread_queue_Acquire( the_thread_queue, &lock_context );
  _Thread_queue_Enqueue_critical(
    the_thread_queue,
    the_thread,
    state,
    timeout,
    &lock_context
  );
}

/**
 *  @brief Extracts thread from thread queue.
 *
 *  This routine removes @a the_thread its thread queue
 *  and cancels any timeouts associated with this blocking.
 *
 *  @param[in] the_thread is the pointer to a thread control block that
 *      is to be removed
 */
void _Thread_queue_Extract( Thread_Control *the_thread );

/**
 *  @brief Extracts thread from thread queue (w/return code).
 *
 *  This routine removes @a the_thread its thread queue
 *  and cancels any timeouts associated with this blocking.
 *
 *  @param[in] the_thread is the pointer to a thread control block that
 *      is to be removed
 *  @param[in] return_code specifies the status to be returned.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Thread_queue_Extract_with_return_code(
  Thread_Control *the_thread,
  uint32_t        return_code
);

/**
 *  @brief Extracts the_thread from the_thread_queue.
 *
 *  This routine extracts the_thread from the_thread_queue
 *  and ensures that if there is a proxy for this task on
 *  another node, it is also dealt with.
 */
void _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
);

/**
 * @brief Returns the first thread on the thread queue if it exists, otherwise
 * @c NULL (locked).
 *
 * The caller must be the owner of the thread queue lock.
 *
 * @param[in] the_thread_queue The thread queue.
 *
 * @retval NULL No thread is present on the thread queue.
 * @retval first The first thread on the thread queue according to the enqueue
 * order.
 */
Thread_Control *_Thread_queue_First_locked(
  Thread_queue_Control *the_thread_queue
);

/**
 * @brief Returns the first thread on the thread queue if it exists, otherwise
 * @c NULL.
 *
 * @param[in] the_thread_queue The thread queue.
 *
 * @retval NULL No thread is present on the thread queue.
 * @retval first The first thread on the thread queue according to the enqueue
 * order.
 */
Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Unblocks all threads blocked on the_thread_queue.
 *
 *  This routine unblocks all threads blocked on the_thread_queue
 *  and cancels any associated timeouts.
 *
 *  @param[in] the_thread_queue is the pointer to a threadq header
 *  @param[in] remote_extract_callout points to a method to invoke to 
 *             invoke when a remote thread is unblocked
 *  @param[in] status is the status which will be returned to
 *             all unblocked threads
 */
void _Thread_queue_Flush(
  Thread_queue_Control       *the_thread_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
);

/**
 *  @brief Initialize the_thread_queue.
 *
 *  This routine initializes the_thread_queue based on the
 *  discipline indicated in attribute_set.  The state set on
 *  threads which block on the_thread_queue is state.
 *
 *  @param[in] the_thread_queue is the pointer to a threadq header
 *  @param[in] the_discipline is the queueing discipline
 *  @param[in] timeout_status is the return on a timeout
 */
void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Thread_queue_Disciplines      the_discipline,
  uint32_t                      timeout_status
);

RTEMS_INLINE_ROUTINE void _Thread_queue_Destroy(
  Thread_queue_Control *the_thread_queue
)
{
  _ISR_lock_Destroy( &the_thread_queue->Lock );
}

/**
 *  @brief Thread queue timeout.
 *
 *  This routine is invoked when a task's request has not
 *  been satisfied after the timeout interval specified to
 *  enqueue.  The task represented by ID will be unblocked and
 *  its status code will be set in it's control block to indicate
 *  that a timeout has occurred.
 *
 *  @param[in] id thread id
 */
void _Thread_queue_Timeout(
  Objects_Id  id,
  void       *ignored
);

/**
 *  @brief Process thread queue timeout.
 *
 * This is a shared helper routine which makes it easier to have multiple
 * object class specific timeout routines.
 *
 * @param[in] the_thread is the thread to extract
 *
 * @note This method assumes thread dispatching is disabled
 *       and is expected to be called via the processing of
 *       a clock tick.
 */
void _Thread_queue_Process_timeout(
  Thread_Control *the_thread
);

/**
 * @brief Compare two thread's priority for RBTree Insertion.
 *
 * @param[in] left points to the left thread's RBnode
 * @param[in] right points to the right thread's RBnode
 *
 * @retval 1 The @a left node is more important than @a right node.
 * @retval 0 The @a left node is of equal importance with @a right node.
 * @retval 1 The @a left node is less important than @a right node.
 */
RBTree_Compare_result _Thread_queue_Compare_priority(
  const RBTree_Node *left,
  const RBTree_Node *right
);

extern const Thread_queue_Operations _Thread_queue_Operations_default;

extern const Thread_queue_Operations _Thread_queue_Operations_FIFO;

extern const Thread_queue_Operations _Thread_queue_Operations_priority;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
