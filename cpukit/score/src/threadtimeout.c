/**
 * @file
 *
 * @brief Thread Wait Timeout
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/threadimpl.h>

static void _Thread_Do_timeout( Thread_Control *the_thread )
{
  the_thread->Wait.return_code = the_thread->Wait.timeout_code;
  ( *the_thread->Wait.operations->extract )(
    the_thread->Wait.queue,
    the_thread
  );
  _Thread_Wait_set_queue( the_thread, NULL );
  _Thread_Wait_restore_default_operations( the_thread );
  _Thread_Lock_restore_default( the_thread );
}

void _Thread_Timeout( Objects_Id id, void *arg )
{
  Thread_Control    *the_thread;
  ISR_lock_Control  *thread_lock;
  ISR_lock_Context   lock_context;
  Thread_Wait_flags  wait_flags;
  bool               unblock;

  the_thread = arg;
  thread_lock = _Thread_Lock_acquire( the_thread, &lock_context );

  wait_flags = _Thread_Wait_flags_get( the_thread );

  if ( ( wait_flags & THREAD_WAIT_STATE_READY_AGAIN ) == 0 ) {
    Thread_Wait_flags wait_class;
    Thread_Wait_flags ready_again;
    bool              success;

    _Thread_Do_timeout( the_thread );

    /*
     * This fence is only necessary for the events, see _Event_Seize().  The
     * thread queues use the thread lock for synchronization.
     */
    _Atomic_Fence( ATOMIC_ORDER_RELEASE );

    wait_class = wait_flags & THREAD_WAIT_CLASS_MASK;
    ready_again = wait_class | THREAD_WAIT_STATE_READY_AGAIN;
    success = _Thread_Wait_flags_try_change_critical(
      the_thread,
      wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK,
      ready_again
    );

    if ( success ) {
      unblock = false;
    } else {
      _Assert(
        _Thread_Wait_flags_get( the_thread )
          == ( wait_class | THREAD_WAIT_STATE_BLOCKED )
      );
      _Thread_Wait_flags_set( the_thread, ready_again );
      unblock = true;
    }
  } else {
    unblock = false;
  }

  _Thread_Lock_release( thread_lock, &lock_context );

  if ( unblock ) {
    _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
      _Thread_MP_Free_proxy( the_thread );
    }
#endif
  }
}
