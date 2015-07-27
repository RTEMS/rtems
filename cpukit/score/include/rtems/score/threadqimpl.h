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
#include <rtems/score/chainimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreThreadQueue
 */
/**@{*/

/**
 * @brief Thread queue with a layout compatible to struct _Thread_queue_Queue
 * defined in Newlib <sys/lock.h>.
 */
typedef struct {
  Thread_queue_Queue Queue;

#if !defined(RTEMS_SMP)
  /*
   * The struct _Thread_queue_Queue definition is independent of the RTEMS
   * build configuration.  Thus, the storage space for the SMP lock is always
   * present.  In SMP configurations, the SMP lock is contained in the
   * Thread_queue_Queue.
   */
  unsigned int reserved[2];
#endif
} Thread_queue_Syslock_queue;

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_initialize(
  Thread_queue_Queue *queue
)
{
  queue->heads = NULL;
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Initialize( &queue->Lock );
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_do_acquire_critical(
  Thread_queue_Queue *queue,
#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  SMP_lock_Stats     *lock_stats,
#endif
  ISR_lock_Context   *lock_context
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Acquire(
    &queue->Lock,
    lock_stats,
    &lock_context->Lock_context.Stats_context
  );
#else
  (void) queue;
  (void) lock_context;
#endif
}

#if defined(RTEMS_SMP) && defined( RTEMS_PROFILING )
  #define \
    _Thread_queue_Queue_acquire_critical( queue, lock_stats, lock_context ) \
    _Thread_queue_Queue_do_acquire_critical( queue, lock_stats, lock_context )
#else
  #define \
    _Thread_queue_Queue_acquire_critical( queue, lock_stats, lock_context ) \
    _Thread_queue_Queue_do_acquire_critical( queue, lock_context )
#endif

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_release(
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Release(
    &queue->Lock,
    &lock_context->Lock_context.Stats_context
  );
#endif
  _ISR_lock_ISR_enable( lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Acquire_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  _Thread_queue_Queue_acquire_critical(
    &the_thread_queue->Queue,
    &the_thread_queue->Lock_stats,
    lock_context
  );
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
  _Thread_queue_Queue_release(
    &the_thread_queue->Queue,
    lock_context
  );
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
 * @brief Blocks the thread and places it on the thread queue.
 *
 * This enqueues the thread on the thread queue, blocks the thread, and
 * optionally starts the thread timer in case the timeout interval is not
 * WATCHDOG_NO_TIMEOUT.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock and register it as the new thread lock.
 * Thread dispatching is disabled before the thread queue lock is released.
 * Thread dispatching is enabled once the sequence to block the thread is
 * complete.  The operation to enqueue the thread on the queue is protected by
 * the thread queue lock.  This makes it possible to use the thread queue lock
 * to protect the state of objects embedding the thread queue and directly
 * enter _Thread_queue_Enqueue_critical() in case the thread must block.
 *
 * @code
 * #include <rtems/score/threadqimpl.h>
 * #include <rtems/score/statesimpl.h>
 *
 * typedef struct {
 *   Thread_queue_Control  Queue;
 *   Thread_Control       *owner;
 * } Mutex;
 *
 * void _Mutex_Obtain( Mutex *mutex )
 * {
 *   ISR_lock_Context  lock_context;
 *   Thread_Control   *executing;
 *
 *   _Thread_queue_Acquire( &mutex->Queue, &lock_context );
 *
 *   executing = _Thread_Executing;
 *
 *   if ( mutex->owner == NULL ) {
 *     mutex->owner = executing;
 *     _Thread_queue_Release( &mutex->Queue, &lock_context );
 *   } else {
 *     _Thread_queue_Enqueue_critical(
 *       &mutex->Queue.Queue,
 *       mutex->Queue.operations,
 *       executing,
 *       STATES_WAITING_FOR_MUTEX,
 *       WATCHDOG_NO_TIMEOUT,
 *       0,
 *       &lock_context
 *     );
 *   }
 * }
 * @endcode
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to enqueue.
 * @param[in] state The new state of the thread.
 * @param[in] timeout Interval to wait.  Use WATCHDOG_NO_TIMEOUT to block
 * potentially forever.
 * @param[in] timeout_code The return code in case a timeout occurs.
 * @param[in] lock_context The lock context of the lock acquire.
 */
void _Thread_queue_Enqueue_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  States_Control                 state,
  Watchdog_Interval              timeout,
  uint32_t                       timeout_code,
  ISR_lock_Context              *lock_context
);

/**
 * @brief Acquires the thread queue lock and calls
 * _Thread_queue_Enqueue_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_queue_Enqueue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  States_Control        state,
  Watchdog_Interval     timeout,
  uint32_t              timeout_code
)
{
  ISR_lock_Context lock_context;

  _Thread_queue_Acquire( the_thread_queue, &lock_context );
  _Thread_queue_Enqueue_critical(
    &the_thread_queue->Queue,
    the_thread_queue->operations,
    the_thread,
    state,
    timeout,
    timeout_code,
    &lock_context
  );
}

/**
 * @brief Extracts the thread from the thread queue, restores the default wait
 * operations and restores the default thread lock.
 *
 * The caller must be the owner of the thread queue lock.  The thread queue
 * lock is not released.
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to extract.
 *
 * @return Returns the unblock indicator for _Thread_queue_Unblock_critical().
 * True indicates, that this thread must be unblocked by the scheduler later in
 * _Thread_queue_Unblock_critical(), and false otherwise.  In case false is
 * returned, then the thread queue enqueue procedure was interrupted.  Thus it
 * will unblock itself and the thread wait information is no longer accessible,
 * since this thread may already block on another resource in an SMP
 * configuration.
 */
bool _Thread_queue_Extract_locked(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread
);

/**
 * @brief Unblocks the thread which was on the thread queue before.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock.  Thread dispatching is disabled before the
 * thread queue lock is released and an unblock is necessary.  Thread
 * dispatching is enabled once the sequence to unblock the thread is complete.
 *
 * @param[in] unblock The unblock indicator returned by
 * _Thread_queue_Extract_locked().
 * @param[in] queue The actual thread queue.
 * @param[in] the_thread The thread to extract.
 * @param[in] lock_context The lock context of the lock acquire.
 */
void _Thread_queue_Unblock_critical(
  bool                unblock,
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  ISR_lock_Context   *lock_context
);

/**
 * @brief Extracts the thread from the thread queue and unblocks it.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock and restore the default thread lock.  Thread
 * dispatching is disabled before the thread queue lock is released and an
 * unblock is necessary.  Thread dispatching is enabled once the sequence to
 * unblock the thread is complete.  This makes it possible to use the thread
 * queue lock to protect the state of objects embedding the thread queue and
 * directly enter _Thread_queue_Extract_critical() to finalize an operation in
 * case a waiting thread exists.
 *
 * @code
 * #include <rtems/score/threadqimpl.h>
 *
 * typedef struct {
 *   Thread_queue_Control  Queue;
 *   Thread_Control       *owner;
 * } Mutex;
 *
 * void _Mutex_Release( Mutex *mutex )
 * {
 *   ISR_lock_Context  lock_context;
 *   Thread_Control   *first;
 *
 *   _Thread_queue_Acquire( &mutex->Queue, &lock_context );
 *
 *   first = _Thread_queue_First_locked( &mutex->Queue );
 *   mutex->owner = first;
 *
 *   if ( first != NULL ) {
 *     _Thread_queue_Extract_critical(
 *       &mutex->Queue.Queue,
 *       mutex->Queue.operations,
 *       first,
 *       &lock_context
 *   );
 * }
 * @endcode
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to extract.
 * @param[in] lock_context The lock context of the lock acquire.
 */
void _Thread_queue_Extract_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  ISR_lock_Context              *lock_context
);

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
 * @c NULL.
 *
 * The caller must be the owner of the thread queue lock.  The thread queue
 * lock is not released.
 *
 * @param[in] the_thread_queue The thread queue.
 *
 * @retval NULL No thread is present on the thread queue.
 * @retval first The first thread on the thread queue according to the enqueue
 * order.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_queue_First_locked(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_queue_Heads *heads = the_thread_queue->Queue.heads;

  if ( heads != NULL ) {
    return ( *the_thread_queue->operations->first )( heads );
  } else {
    return NULL;
  }
}

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
 */
void _Thread_queue_Initialize(
  Thread_queue_Control     *the_thread_queue,
  Thread_queue_Disciplines  the_discipline
);

#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  #define THREAD_QUEUE_FIFO_INITIALIZER( designator, name ) { \
      .Queue = { \
        .heads = NULL, \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
      }, \
      .Lock_stats = SMP_LOCK_STATS_INITIALIZER( name ), \
      .operations = &_Thread_queue_Operations_FIFO \
    }

  #define THREAD_QUEUE_PRIORITY_INITIALIZER( designator, name ) { \
      .Queue = { \
        .heads = NULL, \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
      }, \
      .Lock_stats = SMP_LOCK_STATS_INITIALIZER( name ), \
      .operations = &_Thread_queue_Operations_priority \
    }
#elif defined(RTEMS_SMP)
  #define THREAD_QUEUE_FIFO_INITIALIZER( designator, name ) { \
      .Queue = { \
        .heads = NULL, \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
      }, \
      .operations = &_Thread_queue_Operations_FIFO \
    }

  #define THREAD_QUEUE_PRIORITY_INITIALIZER( designator, name ) { \
      .Queue = { \
        .heads = NULL, \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
      }, \
      .operations = &_Thread_queue_Operations_priority \
    }
#else
  #define THREAD_QUEUE_FIFO_INITIALIZER( designator, name ) { \
      .Queue = { .heads = NULL }, \
      .operations = &_Thread_queue_Operations_FIFO \
    }

  #define THREAD_QUEUE_PRIORITY_INITIALIZER( designator, name ) { \
      .Queue = { .heads = NULL }, \
      .operations = &_Thread_queue_Operations_priority \
    }
#endif

RTEMS_INLINE_ROUTINE void _Thread_queue_Destroy(
  Thread_queue_Control *the_thread_queue
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Destroy( &the_thread_queue->Queue.Lock );
  _SMP_lock_Stats_destroy( &the_thread_queue->Lock_stats );
#endif
}

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
