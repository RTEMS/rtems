/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Suspends Execution of Calling Thread until Target Thread Terminates 
 */

/*
 *  16.1.3 Wait for Thread Termination, P1003.1c/Draft 10, p. 147
 *
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/threadsup.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/statesimpl.h>

static int _POSIX_Threads_Join( pthread_t thread, void **value_ptr )
{
  Thread_Control       *the_thread;
  Thread_queue_Context  queue_context;
  Per_CPU_Control      *cpu_self;
  Thread_Control       *executing;
  void                 *value;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
  the_thread = _Thread_Get( thread, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Per_CPU_Get();
  executing = _Per_CPU_Get_executing( cpu_self );

  if ( executing == the_thread ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    return EDEADLK;
  }

  _Thread_State_acquire_critical(
    the_thread,
    &queue_context.Lock_context.Lock_context
  );

  if ( !_Thread_Is_joinable( the_thread ) ) {
    _Thread_State_release( the_thread, &queue_context.Lock_context.Lock_context );
    return EINVAL;
  }

  if ( _States_Is_waiting_for_join_at_exit( the_thread->current_state ) ) {
    value = the_thread->Life.exit_value;
    _Thread_Clear_state_locked( the_thread, STATES_WAITING_FOR_JOIN_AT_EXIT );
    _Thread_Dispatch_disable_with_CPU(
      cpu_self,
      &queue_context.Lock_context.Lock_context
    );
    _Thread_State_release( the_thread, &queue_context.Lock_context.Lock_context );
    _Thread_Dispatch_direct( cpu_self );
  } else {
    _Thread_Join(
      the_thread,
      STATES_INTERRUPTIBLE_BY_SIGNAL | STATES_WAITING_FOR_JOIN,
      executing,
      &queue_context
    );

    if ( _POSIX_Get_error_after_wait( executing ) != 0 ) {
      _Assert( _POSIX_Get_error_after_wait( executing ) == EINTR );
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
