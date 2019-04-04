/**
 * @file
 *
 * @brief Thread Wait Timeout
 *
 * @ingroup RTEMSScoreThread
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
#include <rtems/score/status.h>

void _Thread_Continue( Thread_Control *the_thread, Status_Control status )
{
  Thread_queue_Context queue_context;
  Thread_Wait_flags    wait_flags;
  bool                 unblock;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Thread_Wait_acquire( the_thread, &queue_context );

  wait_flags = _Thread_Wait_flags_get( the_thread );

  if ( ( wait_flags & THREAD_WAIT_STATE_READY_AGAIN ) == 0 ) {
    Thread_Wait_flags wait_class;
    Thread_Wait_flags ready_again;
    bool              success;

    _Thread_Wait_cancel( the_thread, &queue_context );

    the_thread->Wait.return_code = status;

    wait_class = wait_flags & THREAD_WAIT_CLASS_MASK;
    ready_again = wait_class | THREAD_WAIT_STATE_READY_AGAIN;
    success = _Thread_Wait_flags_try_change_release(
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

  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );

  if ( unblock ) {
    _Thread_Wait_tranquilize( the_thread );
    _Thread_Unblock( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
    if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
      _Thread_MP_Free_proxy( the_thread );
    }
#endif
  }
}

void _Thread_Timeout( Watchdog_Control *the_watchdog )
{
  Thread_Control *the_thread;

  the_thread = RTEMS_CONTAINER_OF(
    the_watchdog,
    Thread_Control,
    Timer.Watchdog
  );
  _Thread_Continue( the_thread, STATUS_TIMEOUT );
}
