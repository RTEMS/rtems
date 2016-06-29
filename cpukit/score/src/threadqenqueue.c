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
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/watchdogimpl.h>

#define THREAD_QUEUE_INTEND_TO_BLOCK \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)

#define THREAD_QUEUE_BLOCKED \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED)

#define THREAD_QUEUE_READY_AGAIN \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_READY_AGAIN)

void _Thread_queue_Enqueue_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  States_Control                 state,
  Watchdog_Interval              timeout,
  Thread_queue_Context          *queue_context
)
{
  Per_CPU_Control *cpu_self;
  bool             success;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet ) {
    the_thread = _Thread_MP_Allocate_proxy( state );
  }
#endif

  _Thread_Lock_set( the_thread, &queue->Lock );

  the_thread->Wait.return_code = STATUS_SUCCESSFUL;
  _Thread_Wait_set_queue( the_thread, queue );
  _Thread_Wait_set_operations( the_thread, operations );

  ( *operations->enqueue )( queue, the_thread );

  _Thread_Wait_flags_set( the_thread, THREAD_QUEUE_INTEND_TO_BLOCK );
  cpu_self = _Thread_Dispatch_disable_critical( &queue_context->Lock_context );
  _Thread_queue_Queue_release( queue, &queue_context->Lock_context );

  if (
    cpu_self->thread_dispatch_disable_level
      != queue_context->expected_thread_dispatch_disable_level
  ) {
    _Terminate(
      INTERNAL_ERROR_CORE,
      false,
      INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_FROM_BAD_STATE
    );
  }

  /*
   *  Set the blocking state for this thread queue in the thread.
   */
  _Thread_Set_state( the_thread, state );

  /*
   *  If the thread wants to timeout, then schedule its timer.
   */
  if ( timeout != WATCHDOG_NO_TIMEOUT ) {
    _Thread_Timer_insert_relative(
      the_thread,
      cpu_self,
      _Thread_Timeout,
      timeout
    );
  }

  /*
   * At this point thread dispatching is disabled, however, we already released
   * the thread queue lock.  Thus, interrupts or threads on other processors
   * may already changed our state with respect to the thread queue object.
   * The request could be satisfied or timed out.  This situation is indicated
   * by the thread wait flags.  Other parties must not modify our thread state
   * as long as we are in the THREAD_QUEUE_INTEND_TO_BLOCK thread wait state,
   * thus we have to cancel the blocking operation ourself if necessary.
   */
  success = _Thread_Wait_flags_try_change_acquire(
    the_thread,
    THREAD_QUEUE_INTEND_TO_BLOCK,
    THREAD_QUEUE_BLOCKED
  );
  if ( !success ) {
    _Thread_Remove_timer_and_unblock( the_thread, queue );
  }

  _Thread_Dispatch_enable( cpu_self );
}

bool _Thread_queue_Do_extract_locked(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread
#if defined(RTEMS_MULTIPROCESSING)
  ,
  const Thread_queue_Context    *queue_context
#endif
)
{
  bool success;
  bool unblock;

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
    Thread_Proxy_control    *the_proxy;
    Thread_queue_MP_callout  mp_callout;

    the_proxy = (Thread_Proxy_control *) the_thread;
    mp_callout = queue_context->mp_callout;
    _Assert( mp_callout != NULL );
    the_proxy->thread_queue_callout = queue_context->mp_callout;
  }
#endif

  ( *operations->extract )( queue, the_thread );

  /*
   * We must update the wait flags under protection of the current thread lock,
   * otherwise a _Thread_Timeout() running on another processor may interfere.
   */
  success = _Thread_Wait_flags_try_change_release(
    the_thread,
    THREAD_QUEUE_INTEND_TO_BLOCK,
    THREAD_QUEUE_READY_AGAIN
  );
  if ( success ) {
    unblock = false;
  } else {
    _Assert( _Thread_Wait_flags_get( the_thread ) == THREAD_QUEUE_BLOCKED );
    _Thread_Wait_flags_set( the_thread, THREAD_QUEUE_READY_AGAIN );
    unblock = true;
  }

  _Thread_Wait_set_queue( the_thread, NULL );
  _Thread_Wait_restore_default_operations( the_thread );
  _Thread_Lock_restore_default( the_thread );

  return unblock;
}

void _Thread_queue_Unblock_critical(
  bool                unblock,
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  ISR_lock_Context   *lock_context
)
{
  if ( unblock ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Thread_queue_Queue_release( queue, lock_context );

    _Thread_Remove_timer_and_unblock( the_thread, queue );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_queue_Queue_release( queue, lock_context );
  }
}

void _Thread_queue_Extract_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
)
{
  bool unblock;

  unblock = _Thread_queue_Extract_locked(
    queue,
    operations,
    the_thread,
    queue_context
  );

  _Thread_queue_Unblock_critical(
    unblock,
    queue,
    the_thread,
    &queue_context->Lock_context
  );
}

void _Thread_queue_Extract( Thread_Control *the_thread )
{
  Thread_queue_Context  queue_context;
  void                 *lock;
  Thread_queue_Queue   *queue;

  _Thread_queue_Context_initialize( &queue_context );
  lock = _Thread_Lock_acquire( the_thread, &queue_context.Lock_context );

  queue = the_thread->Wait.queue;

  if ( queue != NULL ) {
    _SMP_Assert( lock == &queue->Lock );

    _Thread_queue_Context_set_MP_callout(
      &queue_context,
      _Thread_queue_MP_callout_do_nothing
    );
    _Thread_queue_Extract_critical(
      queue,
      the_thread->Wait.operations,
      the_thread,
      &queue_context
    );
  } else {
    _Thread_Lock_release( lock, &queue_context.Lock_context );
  }
}

Thread_Control *_Thread_queue_Do_dequeue(
  Thread_queue_Control          *the_thread_queue,
  const Thread_queue_Operations *operations
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Thread_queue_MP_callout        mp_callout
#endif
)
{
  Thread_queue_Context  queue_context;
  Thread_Control       *the_thread;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_MP_callout( &queue_context, mp_callout );
  _Thread_queue_Acquire( the_thread_queue, &queue_context.Lock_context );

  the_thread = _Thread_queue_First_locked( the_thread_queue, operations );

  if ( the_thread != NULL ) {
    _SMP_Assert( the_thread->Lock.current.normal == &the_thread_queue->Queue.Lock );

    _Thread_queue_Extract_critical(
      &the_thread_queue->Queue,
      operations,
      the_thread,
      &queue_context
    );
  } else {
    _Thread_queue_Release( the_thread_queue, &queue_context.Lock_context );
  }

  return the_thread;
}

#if defined(RTEMS_MULTIPROCESSING)
void _Thread_queue_Unblock_proxy(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  const Thread_queue_Object *the_queue_object;
  Thread_Proxy_control      *the_proxy;
  Thread_queue_MP_callout    mp_callout;

  the_queue_object = THREAD_QUEUE_QUEUE_TO_OBJECT( queue );
  the_proxy = (Thread_Proxy_control *) the_thread;
  mp_callout = the_proxy->thread_queue_callout;
  ( *mp_callout )( the_thread, the_queue_object->Object.id );

  _Thread_MP_Free_proxy( the_thread );
}
#endif
