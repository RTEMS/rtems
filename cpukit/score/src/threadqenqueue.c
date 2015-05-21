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
#include <rtems/score/watchdogimpl.h>

#define THREAD_QUEUE_INTEND_TO_BLOCK \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)

#define THREAD_QUEUE_BLOCKED \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED)

#define THREAD_QUEUE_READY_AGAIN \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_READY_AGAIN)

static void _Thread_queue_Unblock( Thread_Control *the_thread )
{
  _Watchdog_Remove_ticks( &the_thread->Timer );
  _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
    _Thread_MP_Free_proxy( the_thread );
  }
#endif
}

void _Thread_queue_Enqueue_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  States_Control        state,
  Watchdog_Interval     timeout,
  uint32_t              timeout_code,
  ISR_lock_Context     *lock_context
)
{
  const Thread_queue_Operations *operations;
  Per_CPU_Control               *cpu_self;
  bool                           success;

  _Thread_Lock_set( the_thread, &the_thread_queue->Lock );

  operations = the_thread_queue->operations;
  _Thread_Wait_set_queue( the_thread, the_thread_queue );
  _Thread_Wait_set_operations( the_thread, operations );

  ( *operations->enqueue )( the_thread_queue, the_thread );

  _Thread_Wait_flags_set( the_thread, THREAD_QUEUE_INTEND_TO_BLOCK );
  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
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
  if ( timeout != WATCHDOG_NO_TIMEOUT ) {
    _Thread_Wait_set_timeout_code( the_thread, timeout_code );
    _Watchdog_Initialize( &the_thread->Timer, _Thread_Timeout, 0, the_thread );
    _Watchdog_Insert_ticks( &the_thread->Timer, timeout );
  }

  success = _Thread_Wait_flags_try_change(
    the_thread,
    THREAD_QUEUE_INTEND_TO_BLOCK,
    THREAD_QUEUE_BLOCKED
  );
  if ( !success ) {
    _Thread_queue_Unblock( the_thread );
  }

  _Thread_Dispatch_enable( cpu_self );
}

void _Thread_queue_Extract_locked(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  ( *the_thread_queue->operations->extract )( the_thread_queue, the_thread );

  _Thread_Wait_set_queue( the_thread, NULL );
  _Thread_Wait_restore_default_operations( the_thread );
  _Thread_Lock_restore_default( the_thread );
}

void _Thread_queue_Unblock_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_lock_Context     *lock_context
)
{
  bool success;
  bool unblock;

  success = _Thread_Wait_flags_try_change_critical(
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

  if ( unblock ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Thread_queue_Release( the_thread_queue, lock_context );

    _Thread_queue_Unblock( the_thread );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_queue_Release( the_thread_queue, lock_context );
  }
}

void _Thread_queue_Extract_critical(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_lock_Context     *lock_context
)
{
  _Thread_queue_Extract_locked( the_thread_queue, the_thread );
  _Thread_queue_Unblock_critical( the_thread_queue, the_thread, lock_context );
}

void _Thread_queue_Extract( Thread_Control *the_thread )
{
  ISR_lock_Context      lock_context;
  ISR_lock_Control     *lock;
  Thread_queue_Control *the_thread_queue;

  lock = _Thread_Lock_acquire( the_thread, &lock_context );

  the_thread_queue = the_thread->Wait.queue;

  if ( the_thread_queue != NULL ) {
    _SMP_Assert( lock == &the_thread_queue->Lock );

    _Thread_queue_Extract_critical( the_thread_queue, the_thread, &lock_context );
  } else {
    _Thread_Lock_release( lock, &lock_context );
  }
}

Thread_Control *_Thread_queue_Dequeue( Thread_queue_Control *the_thread_queue )
{
  ISR_lock_Context  lock_context;
  Thread_Control   *the_thread;

  _Thread_queue_Acquire( the_thread_queue, &lock_context );

  the_thread = _Thread_queue_First_locked( the_thread_queue );

  if ( the_thread != NULL ) {
    _SMP_Assert( the_thread->Lock.current == &the_thread_queue->Lock );

    _Thread_queue_Extract_critical( the_thread_queue, the_thread, &lock_context );
  } else {
    _Thread_queue_Release( the_thread_queue, &lock_context );
  }

  return the_thread;
}
