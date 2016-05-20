/**
 * @file
 *
 * @brief Suspends Execution of Calling Thread until Target Thread Terminates 
 * @ingroup POSIXAPI
 */

/*
 *  16.1.3 Wait for Thread Termination, P1003.1c/Draft 10, p. 147
 *
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/threadsup.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/statesimpl.h>

static int _POSIX_Threads_Join( pthread_t thread, void **value_ptr )
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  Per_CPU_Control   *cpu_self;
  Thread_Control    *executing;
  void              *value;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Per_CPU_Get();
  executing = _Per_CPU_Get_executing( cpu_self );

  if ( executing == the_thread ) {
    _ISR_lock_ISR_enable( &lock_context );
    return EDEADLK;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  if ( !_Thread_Is_joinable( the_thread ) ) {
    _Thread_State_release( the_thread, &lock_context );
    return EINVAL;
  }

  if ( _States_Is_waiting_for_join_at_exit( the_thread->current_state ) ) {
    value = the_thread->Life.exit_value;
    _Thread_Clear_state_locked( the_thread, STATES_WAITING_FOR_JOIN_AT_EXIT );
    _Thread_Dispatch_disable_with_CPU( cpu_self, &lock_context );
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_Join(
      the_thread,
      STATES_INTERRUPTIBLE_BY_SIGNAL | STATES_WAITING_FOR_JOIN,
      executing,
      &lock_context
    );

    if ( executing->Wait.return_code != 0 ) {
      _Assert( executing->Wait.return_code == EINTR );
      return EINTR;
    }

    value = executing->Wait.return_argument;
  }

  if ( value_ptr != NULL ) {
    *value_ptr = value;
  }

  return 0;
}

int pthread_join( pthread_t thread, void **value_ptr )
{
  int error;

  do {
    error = _POSIX_Threads_Join( thread, value_ptr );
  } while ( error == EINTR );

  return error;
}
