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
#include <rtems/score/priorityimpl.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/smp.h>
#include <rtems/score/status.h>
#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>

#if defined(RTEMS_DEBUG)
#include <string.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreThreadQueue
 */
/**@{*/

#define THREAD_QUEUE_LINK_OF_PATH_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Thread_queue_Link, Path_node );

/**
 * @brief Thread queue with a layout compatible to struct _Thread_queue_Queue
 * defined in Newlib <sys/lock.h>.
 */
typedef struct {
#if !defined(RTEMS_SMP)
  /*
   * The struct _Thread_queue_Queue definition is independent of the RTEMS
   * build configuration.  Thus, the storage space for the SMP lock is always
   * present.  In SMP configurations, the SMP lock is contained in the
   * Thread_queue_Queue.
   */
  unsigned int reserved[2];
#endif

  Thread_queue_Queue Queue;
} Thread_queue_Syslock_queue;

void _Thread_queue_Enqueue_do_nothing_extra(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
);

void _Thread_queue_Add_timeout_ticks(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
);

void _Thread_queue_Add_timeout_monotonic_timespec(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
);

void _Thread_queue_Add_timeout_realtime_timespec(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
);

/**
 * @brief Sets the thread wait return code to STATUS_DEADLOCK.
 */
void _Thread_queue_Deadlock_status( Thread_Control *the_thread );

/**
 * @brief Results in an INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK fatal error.
 */
void _Thread_queue_Deadlock_fatal( Thread_Control *the_thread );

/**
 * @brief Initializes a thread queue context.
 *
 * @param queue_context The thread queue context to initialize.
 */
RTEMS_INLINE_ROUTINE void _Thread_queue_Context_initialize(
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_DEBUG)
  memset( queue_context, 0x7f, sizeof( *queue_context ) );
#if defined(RTEMS_SMP)
  _Chain_Initialize_node( &queue_context->Lock_context.Wait.Gate.Node );
#endif
  queue_context->enqueue_callout = NULL;
  queue_context->deadlock_callout = NULL;
#else
  (void) queue_context;
#endif
}

/**
 * @brief Sets the thread state for the thread to enqueue in the thread queue
 * context.
 *
 * @param queue_context The thread queue context.
 * @param state The thread state.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_thread_state(
  Thread_queue_Context *queue_context,
  States_Control        thread_state
)
{
  queue_context->thread_state = thread_state;
}

/**
 * @brief Sets the timeout ticks in the thread queue context.
 *
 * @param queue_context The thread queue context.
 * @param ticks The timeout in ticks.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_timeout_ticks(
  Thread_queue_Context *queue_context,
  Watchdog_Interval     ticks
)
{
  queue_context->Timeout.ticks = ticks;
}

/**
 * @brief Sets the timeout argument in the thread queue context.
 *
 * @param queue_context The thread queue context.
 * @param arg The timeout argument.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_timeout_argument(
  Thread_queue_Context *queue_context,
  const void           *arg
)
{
  queue_context->Timeout.arg = arg;
}

/**
 * @brief Sets the enqueue callout in the thread queue context.
 *
 * @param queue_context The thread queue context.
 * @param enqueue_callout The enqueue callout.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_enqueue_callout(
  Thread_queue_Context         *queue_context,
  Thread_queue_Enqueue_callout  enqueue_callout
)
{
  queue_context->enqueue_callout = enqueue_callout;
}

/**
 * @brief Sets the do nothing enqueue callout in the thread queue context.
 *
 * @param queue_context The thread queue context.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_enqueue_do_nothing_extra(
  Thread_queue_Context *queue_context
)
{
  queue_context->enqueue_callout = _Thread_queue_Enqueue_do_nothing_extra;
}

/**
 * @brief Sets the enqueue callout to add a relative monotonic timeout in
 * ticks.
 *
 * @param queue_context The thread queue context.
 * @param ticks The timeout in ticks.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_enqueue_timeout_ticks(
  Thread_queue_Context *queue_context,
  Watchdog_Interval     ticks
)
{
  queue_context->Timeout.ticks = ticks;
  queue_context->enqueue_callout = _Thread_queue_Add_timeout_ticks;
}

/**
 * @brief Sets the enqueue callout to add an absolute monotonic timeout in
 * timespec format.
 *
 * @param queue_context The thread queue context.
 * @param abstime The absolute monotonic timeout.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_enqueue_timeout_monotonic_timespec(
  Thread_queue_Context  *queue_context,
  const struct timespec *abstime
)
{
  queue_context->Timeout.arg = abstime;
  queue_context->enqueue_callout =
    _Thread_queue_Add_timeout_monotonic_timespec;
}

/**
 * @brief Sets the enqueue callout to add an absolute realtime timeout in
 * timespec format.
 *
 * @param queue_context The thread queue context.
 * @param abstime The absolute realtime timeout.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void
_Thread_queue_Context_set_enqueue_timeout_realtime_timespec(
  Thread_queue_Context  *queue_context,
  const struct timespec *abstime
)
{
  queue_context->Timeout.arg = abstime;
  queue_context->enqueue_callout = _Thread_queue_Add_timeout_realtime_timespec;
}

/**
 * @brief Sets the deadlock callout in the thread queue
 * context.
 *
 * A deadlock callout must be provided for _Thread_queue_Enqueue()
 * operations that operate on thread queues which may have an owner, e.g. mutex
 * objects.  Available deadlock callouts are _Thread_queue_Deadlock_status()
 * and _Thread_queue_Deadlock_fatal().
 *
 * @param queue_context The thread queue context.
 * @param deadlock_callout The deadlock callout.
 *
 * @see _Thread_queue_Enqueue().
 */
RTEMS_INLINE_ROUTINE void _Thread_queue_Context_set_deadlock_callout(
  Thread_queue_Context          *queue_context,
  Thread_queue_Deadlock_callout  deadlock_callout
)
{
  queue_context->deadlock_callout = deadlock_callout;
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Context_clear_priority_updates(
  Thread_queue_Context *queue_context
)
{
  queue_context->Priority.update_count = 0;
}

RTEMS_INLINE_ROUTINE size_t _Thread_queue_Context_save_priority_updates(
  Thread_queue_Context *queue_context
)
{
  return queue_context->Priority.update_count;
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Context_restore_priority_updates(
  Thread_queue_Context *queue_context,
  size_t                update_count
)
{
  queue_context->Priority.update_count = update_count;
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Context_add_priority_update(
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  size_t n;

  n = queue_context->Priority.update_count;
  _Assert( n < RTEMS_ARRAY_SIZE( queue_context->Priority.update ) );

  queue_context->Priority.update_count = n + 1;
  queue_context->Priority.update[ n ] = the_thread;
}

#define _Thread_queue_Context_ISR_disable( queue_context, level ) \
  do { \
    _ISR_Local_disable( level ); \
    _ISR_lock_ISR_disable_profile( \
      &( queue_context )->Lock_context.Lock_context \
    ) \
  } while ( 0 )

RTEMS_INLINE_ROUTINE void _Thread_queue_Context_set_ISR_level(
  Thread_queue_Context *queue_context,
  ISR_Level             level
)
{
  _ISR_lock_Context_set_level(
    &queue_context->Lock_context.Lock_context,
    level
  );
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_queue_Dispatch_disable(
  Thread_queue_Context *queue_context
)
{
  return _Thread_Dispatch_disable_critical(
    &queue_context->Lock_context.Lock_context
  );
}

/**
 * @brief Sets the MP callout in the thread queue context.
 *
 * @param queue_context The thread queue context.
 * @param mp_callout Callout to unblock the thread in case it is actually a
 *   thread proxy.  This parameter is only used on multiprocessing
 *   configurations.  Used by thread queue extract and unblock methods for
 *   objects with multiprocessing (MP) support.
 */
#if defined(RTEMS_MULTIPROCESSING)
RTEMS_INLINE_ROUTINE void _Thread_queue_Context_set_MP_callout(
  Thread_queue_Context    *queue_context,
  Thread_queue_MP_callout  mp_callout
)
{
  queue_context->mp_callout = mp_callout;
}
#else
#define _Thread_queue_Context_set_MP_callout( queue_context, mp_callout ) \
  do { \
    (void) queue_context; \
  } while ( 0 )
#endif

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE void _Thread_queue_Gate_close(
  Thread_queue_Gate *gate
)
{
  _Atomic_Store_uint( &gate->go_ahead, 0, ATOMIC_ORDER_RELAXED );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Gate_add(
  Chain_Control     *chain,
  Thread_queue_Gate *gate
)
{
  _Chain_Append_unprotected( chain, &gate->Node );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Gate_open(
  Thread_queue_Gate *gate
)
{
  _Atomic_Store_uint( &gate->go_ahead, 1, ATOMIC_ORDER_RELAXED );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Gate_wait(
  Thread_queue_Gate *gate
)
{
  while ( _Atomic_Load_uint( &gate->go_ahead, ATOMIC_ORDER_RELAXED ) == 0 ) {
    /* Wait */
  }
}
#endif

RTEMS_INLINE_ROUTINE void _Thread_queue_Heads_initialize(
  Thread_queue_Heads *heads
)
{
#if defined(RTEMS_SMP)
  size_t i;

  for ( i = 0; i < _Scheduler_Count; ++i ) {
    _Chain_Initialize_node( &heads->Priority[ i ].Node );
    _Priority_Initialize_empty( &heads->Priority[ i ].Queue );
    heads->Priority[ i ].Queue.scheduler = &_Scheduler_Table[ i ];
  }
#endif

  _Chain_Initialize_empty( &heads->Free_chain );
  _Chain_Initialize_node( &heads->Free_node );
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_initialize(
  Thread_queue_Queue *queue,
  const char         *name
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Initialize( &queue->Lock );
#endif
  queue->heads = NULL;
  queue->owner = NULL;
  queue->name = name;
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

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_release_critical(
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Release(
    &queue->Lock,
    &lock_context->Lock_context.Stats_context
  );
#else
  (void) queue;
  (void) lock_context;
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_queue_Queue_release(
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
  _Thread_queue_Queue_release_critical( queue, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

/**
 * @brief Copies the thread queue name to the specified buffer.
 *
 * @param[in] queue The actual thread queue.
 * @param[in] buffer The buffer for the thread queue name copy.
 * @param[in] buffer_size The buffer size in characters.
 * @param[in] id The object identifier in case the thread queue is embedded in
 *   an object with identifier, otherwise it is set to 0.
 *
 * @retval The length of the thread queue name.  May be greater than or equal
 *   to the buffer size if truncation occurred.
 */
size_t _Thread_queue_Queue_get_name_and_id(
  const Thread_queue_Queue *queue,
  char                     *buffer,
  size_t                    buffer_size,
  Objects_Id               *id
);

#if defined(RTEMS_SMP)
void _Thread_queue_Do_acquire_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
);
#else
RTEMS_INLINE_ROUTINE void _Thread_queue_Do_acquire_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  (void) the_thread_queue;
  (void) lock_context;
}
#endif

RTEMS_INLINE_ROUTINE void _Thread_queue_Acquire_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Do_acquire_critical(
    the_thread_queue,
    &queue_context->Lock_context.Lock_context
  );
}

#if defined(RTEMS_SMP)
void _Thread_queue_Acquire(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
);
#else
RTEMS_INLINE_ROUTINE void _Thread_queue_Acquire(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
  (void) the_thread_queue;
  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
}
#endif

#if defined(RTEMS_DEBUG)
RTEMS_INLINE_ROUTINE bool _Thread_queue_Is_lock_owner(
  const Thread_queue_Control *the_thread_queue
)
{
#if defined(RTEMS_SMP)
  return the_thread_queue->owner == _SMP_lock_Who_am_I();
#else
  return _ISR_Get_level() != 0;
#endif
}
#endif

#if defined(RTEMS_SMP)
void _Thread_queue_Do_release_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
);
#else
RTEMS_INLINE_ROUTINE void _Thread_queue_Do_release_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  (void) the_thread_queue;
  (void) lock_context;
  _Assert( _Thread_queue_Is_lock_owner( the_thread_queue ) );
}
#endif

RTEMS_INLINE_ROUTINE void _Thread_queue_Release_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Do_release_critical(
    the_thread_queue,
    &queue_context->Lock_context.Lock_context
  );
}

#if defined(RTEMS_SMP)
void _Thread_queue_Release(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
);
#else
RTEMS_INLINE_ROUTINE void _Thread_queue_Release(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
  (void) the_thread_queue;
  _Assert( _Thread_queue_Is_lock_owner( the_thread_queue ) );
  _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
}
#endif

Thread_Control *_Thread_queue_Do_dequeue(
  Thread_queue_Control          *the_thread_queue,
  const Thread_queue_Operations *operations
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Thread_queue_MP_callout        mp_callout
#endif
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
#if defined(RTEMS_MULTIPROCESSING)
  #define _Thread_queue_Dequeue( \
    the_thread_queue, \
    operations, \
    mp_callout \
  ) \
    _Thread_queue_Do_dequeue( \
      the_thread_queue, \
      operations, \
      mp_callout \
    )
#else
  #define _Thread_queue_Dequeue( \
    the_thread_queue, \
    operations, \
    mp_callout \
  ) \
    _Thread_queue_Do_dequeue( \
      the_thread_queue, \
      operations \
    )
#endif

/**
 * @brief Blocks the thread and places it on the thread queue.
 *
 * This enqueues the thread on the thread queue, blocks the thread, and
 * optionally starts the thread timer in case the timeout discipline is not
 * WATCHDOG_NO_TIMEOUT. Timeout discipline and value are in the queue_context.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock and register it as the new thread lock.
 * Thread dispatching is disabled before the thread queue lock is released.
 * Thread dispatching is enabled once the sequence to block the thread is
 * complete.  The operation to enqueue the thread on the queue is protected by
 * the thread queue lock.  This makes it possible to use the thread queue lock
 * to protect the state of objects embedding the thread queue and directly
 * enter _Thread_queue_Enqueue() in case the thread must block.
 *
 * The thread queue context must be set up with the following functions,
 * otherwise the behaviour is unpredictable
 *
 * - _Thread_queue_Context_set_thread_state(),
 *
 * - _Thread_queue_Context_set_enqueue_callout() or
 *   _Thread_queue_Context_set_enqueue_do_nothing_extra() or
 *   _Thread_queue_Context_set_enqueue_timeout_ticks() or
 *   _Thread_queue_Context_set_enqueue_timeout_monotonic_timespec() or
 *   _Thread_queue_Context_set_enqueue_timeout_realtime_timespec(),
 *
 * - _Thread_queue_Context_set_deadlock_callout().
 *
 * @code
 * #include <rtems/score/threadqimpl.h>
 * #include <rtems/score/statesimpl.h>
 *
 * #define MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority
 *
 * typedef struct {
 *   Thread_queue_Control Queue;
 * } Mutex;
 *
 * void _Mutex_Obtain( Mutex *mutex )
 * {
 *   Thread_queue_Context  queue_context;
 *   Thread_Control       *executing;
 *
 *   _Thread_queue_Context_initialize( &queue_context );
 *   _Thread_queue_Acquire( &mutex->Queue, queue_context );
 *
 *   executing = _Thread_Executing;
 *
 *   if ( mutex->Queue.owner == NULL ) {
 *     mutex->Queue.owner = executing;
 *     _Thread_queue_Release( &mutex->Queue, queue_context );
 *   } else {
 *     _Thread_queue_Context_set_thread_state(
 *       &queue_context,
 *       STATES_WAITING_FOR_MUTEX
 *     );
 *     _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
 *     _Thread_queue_Context_set_deadlock_callout(
 *       queue_context,
 *       _Thread_queue_Deadlock_fatal
 *     );
 *     _Thread_queue_Enqueue(
 *       &mutex->Queue.Queue,
 *       MUTEX_TQ_OPERATIONS,
 *       executing,
 *       &queue_context
 *     );
 *   }
 * }
 * @endcode
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to enqueue.
 * @param[in] queue_context The thread queue context of the lock acquire.
 */
void _Thread_queue_Enqueue(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
);

#if defined(RTEMS_SMP)
/**
 * @brief Enqueues the thread on the thread queue and busy waits for dequeue.
 *
 * Optionally starts the thread timer in case the timeout discipline is not
 * WATCHDOG_NO_TIMEOUT. Timeout discipline and value are in the queue_context.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue lock and register it as the new thread lock.
 *
 * The thread priorities of the owner and the are updated with respect to the
 * scheduler.  The sticky level of the thread is incremented.  A thread
 * dispatch is performed if necessary.
 *
 * Afterwards, the thread busy waits on the thread wait flags until a timeout
 * occurs or the thread queue is surrendered to this thread.  So, it sticks to
 * the processor instead of blocking with respect to the scheduler.
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to enqueue.
 * @param[in] queue_context The thread queue context of the lock acquire.
 */
Status_Control _Thread_queue_Enqueue_sticky(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
);
#endif

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
 * @param[in] queue_context The thread queue context.
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
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
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
 *   Thread_queue_Context  queue_context;
 *   Thread_Control       *first;
 *
 *   _Thread_queue_Context_initialize( &queue_context, NULL );
 *   _Thread_queue_Acquire( &mutex->Queue, queue_context );
 *
 *   first = _Thread_queue_First_locked( &mutex->Queue );
 *   mutex->owner = first;
 *
 *   if ( first != NULL ) {
 *     _Thread_queue_Extract_critical(
 *       &mutex->Queue.Queue,
 *       mutex->Queue.operations,
 *       first,
 *       &queue_context
 *   );
 * }
 * @endcode
 *
 * @param[in] queue The actual thread queue.
 * @param[in] operations The thread queue operations.
 * @param[in] the_thread The thread to extract.
 * @param[in] queue_context The thread queue context of the lock acquire.
 */
void _Thread_queue_Extract_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
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
 * @brief Surrenders the thread queue previously owned by the thread to the
 * first enqueued thread.
 *
 * The owner of the thread queue must be set to NULL by the caller.
 *
 * This function releases the thread queue lock.  In addition it performs a
 * thread dispatch if necessary.
 *
 * @param[in] queue The actual thread queue.
 * @param[in] heads The thread queue heads.  It must not be NULL.
 * @param[in] previous_owner The previous owner thread surrendering the thread
 *   queue.
 * @param[in] queue_context The thread queue context of the lock acquire.
 * @param[in] operations The thread queue operations.
 */
void _Thread_queue_Surrender(
  Thread_queue_Queue            *queue,
  Thread_queue_Heads            *heads,
  Thread_Control                *previous_owner,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
);

#if defined(RTEMS_SMP)
/**
 * @brief Surrenders the thread queue previously owned by the thread to the
 * first enqueued thread.
 *
 * The owner of the thread queue must be set to NULL by the caller.
 *
 * The caller must be the owner of the thread queue lock.  This function will
 * release the thread queue.
 *
 * The thread priorities of the previous owner and the new owner are updated.  The
 * sticky level of the previous owner is decremented.  A thread dispatch is
 * performed if necessary.
 *
 * @param[in] queue The actual thread queue.
 * @param[in] heads The thread queue heads.  It must not be NULL.
 * @param[in] previous_owner The previous owner thread surrendering the thread
 *   queue.
 * @param[in] queue_context The thread queue context of the lock acquire.
 * @param[in] operations The thread queue operations.
 */
void _Thread_queue_Surrender_sticky(
  Thread_queue_Queue            *queue,
  Thread_queue_Heads            *heads,
  Thread_Control                *previous_owner,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
);
#endif

RTEMS_INLINE_ROUTINE bool _Thread_queue_Is_empty(
  const Thread_queue_Queue *queue
)
{
  return queue->heads == NULL;
}

/**
 * @brief Returns the first thread on the thread queue if it exists, otherwise
 * @c NULL.
 *
 * The caller must be the owner of the thread queue lock.  The thread queue
 * lock is not released.
 *
 * @param[in] the_thread_queue The thread queue.
 * @param[in] operations The thread queue operations.
 *
 * @retval NULL No thread is present on the thread queue.
 * @retval first The first thread on the thread queue according to the enqueue
 * order.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_queue_First_locked(
  Thread_queue_Control          *the_thread_queue,
  const Thread_queue_Operations *operations
)
{
  Thread_queue_Heads *heads = the_thread_queue->Queue.heads;

  if ( heads != NULL ) {
    return ( *operations->first )( heads );
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
  Thread_queue_Control          *the_thread_queue,
  const Thread_queue_Operations *operations
);

/**
 * @brief Thread queue flush filter function.
 *
 * Called under protection of the thread queue lock by
 * _Thread_queue_Flush_critical() to optionally alter the thread wait
 * information and control the iteration.
 *
 * @param the_thread The thread to extract.  This is the first parameter to
 *   optimize for architectures that use the same register for the first
 *   parameter and the return value.
 * @param queue The actual thread queue.
 * @param queue_context The thread queue context of the lock acquire.  May be
 *   used to pass additional data to the filter function via an overlay
 *   structure.  The filter function should not release or acquire the thread
 *   queue lock.
 *
 * @retval the_thread Extract this thread.
 * @retval NULL Do not extract this thread and stop the thread queue flush
 *   operation.  Threads that are already extracted will complete the flush
 *   operation.
 */
typedef Thread_Control *( *Thread_queue_Flush_filter )(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
);

/**
 * @brief Default thread queue flush filter function.
 *
 * @param the_thread The thread to extract.
 * @param queue Unused.
 * @param queue_context Unused.
 *
 * @retval the_thread Extract this thread.
 */
Thread_Control *_Thread_queue_Flush_default_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
);

/**
 * @brief Status unavailable thread queue flush filter function.
 *
 * Sets the thread wait return code of the thread to STATUS_UNAVAILABLE.
 *
 * @param the_thread The thread to extract.
 * @param queue Unused.
 * @param queue_context Unused.
 *
 * @retval the_thread Extract this thread.
 */
Thread_Control *_Thread_queue_Flush_status_unavailable(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
);

/**
 * @brief Status object was deleted thread queue flush filter function.
 *
 * Sets the thread wait return code of the thread to STATUS_OBJECT_WAS_DELETED
 *
 * @param the_thread The thread to extract.
 * @param queue Unused.
 * @param queue_context Unused.
 *
 * @retval the_thread Extract this thread.
 */
Thread_Control *_Thread_queue_Flush_status_object_was_deleted(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
);

/**
 * @brief Unblocks all threads enqueued on the thread queue.
 *
 * This function iteratively extracts the first enqueued thread of the thread
 * queue until the thread queue is empty or the filter function indicates a
 * stop.  The thread timers of the extracted threads are cancelled.  The
 * extracted threads are unblocked.
 *
 * @param queue The actual thread queue.
 * @param operations The thread queue operations.
 * @param filter The filter functions is called for each thread to extract from
 *   the thread queue.  It may be used to alter the thread under protection of
 *   the thread queue lock, for example to set the thread wait return code.
 *   The return value of the filter function controls if the thread queue flush
 *   operation should stop or continue.
 * @param queue_context The thread queue context of the lock acquire.  May be
 *   used to pass additional data to the filter function via an overlay
 *   structure.  The filter function should not release or acquire the thread
 *   queue lock.
 *
 * @return The count of extracted threads.
 */
size_t _Thread_queue_Flush_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_queue_Flush_filter      filter,
  Thread_queue_Context          *queue_context
);

void _Thread_queue_Initialize(
  Thread_queue_Control *the_thread_queue,
  const char           *name
);

#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG) && defined(RTEMS_PROFILING)
  #define THREAD_QUEUE_INITIALIZER( _name ) \
    { \
      .Lock_stats = SMP_LOCK_STATS_INITIALIZER( _name ), \
      .owner = SMP_LOCK_NO_OWNER, \
      .Queue = { \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
        .heads = NULL, \
        .owner = NULL, \
        .name = _name \
      } \
    }
#elif defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
  #define THREAD_QUEUE_INITIALIZER( _name ) \
    { \
      .owner = SMP_LOCK_NO_OWNER, \
      .Queue = { \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
        .heads = NULL, \
        .owner = NULL, \
        .name = _name \
      } \
    }
#elif defined(RTEMS_SMP) && defined(RTEMS_PROFILING)
  #define THREAD_QUEUE_INITIALIZER( _name ) \
    { \
      .Lock_stats = SMP_LOCK_STATS_INITIALIZER( _name ), \
      .Queue = { \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
        .heads = NULL, \
        .owner = NULL, \
        .name = _name \
      } \
    }
#elif defined(RTEMS_SMP)
  #define THREAD_QUEUE_INITIALIZER( _name ) \
    { \
      .Queue = { \
        .Lock = SMP_TICKET_LOCK_INITIALIZER, \
        .heads = NULL, \
        .owner = NULL, \
        .name = _name \
      } \
    }
#else
  #define THREAD_QUEUE_INITIALIZER( _name ) \
    { \
      .Queue = { \
        .heads = NULL, \
        .owner = NULL, \
        .name = _name \
      } \
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

#if defined(RTEMS_MULTIPROCESSING)
void _Thread_queue_MP_callout_do_nothing(
  Thread_Control *the_proxy,
  Objects_Id      mp_id
);

void _Thread_queue_Unblock_proxy(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
);
#endif

#if defined(RTEMS_SMP)
bool _Thread_queue_Path_acquire_critical(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
);

void _Thread_queue_Path_release_critical(
  Thread_queue_Context *queue_context
);
#endif

/**
 * @brief Helper structure to ensure that all objects containing a thread queue
 * have the right layout.
 *
 * @see _Thread_Wait_get_id() and THREAD_QUEUE_OBJECT_ASSERT().
 */
typedef struct {
  Objects_Control      Object;
  Thread_queue_Control Wait_queue;
} Thread_queue_Object;

#define THREAD_QUEUE_OBJECT_ASSERT( object_type, wait_queue_member, msg ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( object_type, wait_queue_member ) \
      == offsetof( Thread_queue_Object, Wait_queue ) \
    && RTEMS_HAVE_MEMBER_SAME_TYPE( \
      object_type, \
      wait_queue_member, \
      Thread_queue_Object, \
      Wait_queue \
    ), \
    msg \
  )

#define THREAD_QUEUE_QUEUE_TO_OBJECT( queue ) \
  RTEMS_CONTAINER_OF( \
    queue, \
    Thread_queue_Object, \
    Wait_queue.Queue \
  )

extern const Thread_queue_Operations _Thread_queue_Operations_default;

extern const Thread_queue_Operations _Thread_queue_Operations_FIFO;

extern const Thread_queue_Operations _Thread_queue_Operations_priority;

extern const Thread_queue_Operations _Thread_queue_Operations_priority_inherit;

/**
 * @brief The special thread queue name to indicated that the thread queue is
 * embedded in an object with identifier.
 *
 * @see _Thread_queue_Object_initialize().
 */
extern const char _Thread_queue_Object_name[];

/**
 * @brief Initializes a thread queue embedded in an object with identifier.
 *
 * The object must have the layout specified by Thread_queue_Object.  It should
 * be ensured with the THREAD_QUEUE_OBJECT_ASSERT() static assertion.
 *
 * @param[in] the_thread_queue The thread queue.
 */
void _Thread_queue_Object_initialize(
  Thread_queue_Control *the_thread_queue
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
