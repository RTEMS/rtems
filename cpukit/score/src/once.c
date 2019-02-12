/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
 * Copyright (C) 2019 Sebastian Huber
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

#include <rtems/score/onceimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/thread.h>

#define ONCE_STATE_INIT 0

#define ONCE_STATE_RUNNING 1

#define ONCE_STATE_COMPLETE 2

typedef struct {
  rtems_mutex              Mutex;
  rtems_condition_variable State;
} Once_Control;

static Once_Control _Once_Information = {
  .Mutex = RTEMS_MUTEX_INITIALIZER( "_Once" ),
  .State = RTEMS_CONDITION_VARIABLE_INITIALIZER( "_Once" )
};

int _Once( unsigned char *once_state, void ( *init_routine )( void ) )
{
  _Atomic_Fence( ATOMIC_ORDER_ACQUIRE );

  if ( RTEMS_PREDICT_FALSE( *once_state != ONCE_STATE_COMPLETE ) ) {
    Thread_Life_state thread_life_state;

    thread_life_state = _Once_Lock();

    if ( *once_state == ONCE_STATE_INIT ) {
      *once_state = ONCE_STATE_RUNNING;
      _Once_Unlock( THREAD_LIFE_PROTECTED );
      ( *init_routine )();
      _Once_Lock();
      _Atomic_Fence( ATOMIC_ORDER_RELEASE );
      *once_state = ONCE_STATE_COMPLETE;
      rtems_condition_variable_broadcast( &_Once_Information.State );
    } else {
      while ( *once_state != ONCE_STATE_COMPLETE ) {
        rtems_condition_variable_wait(
          &_Once_Information.State,
          &_Once_Information.Mutex
        );
      }
    }

    _Once_Unlock( thread_life_state );
  }

  return 0;
}

Thread_Life_state _Once_Lock( void )
{
  Thread_Life_state thread_life_state;

  thread_life_state = _Thread_Set_life_protection( THREAD_LIFE_PROTECTED );
  rtems_mutex_lock( &_Once_Information.Mutex );

  return thread_life_state;
}

void _Once_Unlock( Thread_Life_state thread_life_state )
{
  rtems_mutex_unlock( &_Once_Information.Mutex );
  _Thread_Set_life_protection( thread_life_state );
}
