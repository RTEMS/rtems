/**
 *  @file
 *
 *  @brief Timeout Event
 *  @ingroup ClassicEvent
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

#include <rtems/rtems/eventimpl.h>
#include <rtems/score/threadimpl.h>

void _Event_Timeout(
  Objects_Id  id,
  void       *arg
)
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  Thread_Wait_flags  wait_flags;
  Thread_Wait_flags  wait_class;
  Thread_Wait_flags  intend_to_block;
  Thread_Wait_flags  blocked;
  bool               success;
  bool               unblock;

  the_thread = arg;
  _Thread_Lock_acquire_default( the_thread, &lock_context );

  wait_flags = _Thread_Wait_flags_get( the_thread );
  wait_class = wait_flags & THREAD_WAIT_CLASS_MASK;
  intend_to_block = wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK;
  blocked = wait_class | THREAD_WAIT_STATE_BLOCKED;
  success = _Thread_Wait_flags_try_change_critical(
    the_thread,
    intend_to_block,
    wait_class | THREAD_WAIT_STATE_READY_AGAIN
  );

  if ( success ) {
    the_thread->Wait.return_code = RTEMS_TIMEOUT;
    unblock = false;
  } else if ( _Thread_Wait_flags_get( the_thread ) == blocked ) {
    the_thread->Wait.return_code = RTEMS_TIMEOUT;
    _Thread_Wait_flags_set(
      the_thread,
      wait_class | THREAD_WAIT_STATE_READY_AGAIN
    );
    unblock = true;
  } else {
    unblock = false;
  }

  _Thread_Lock_release_default( the_thread, &lock_context );

  if ( unblock ) {
    _Thread_Unblock( the_thread );
  }
}
