/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Clear_state_locked() and _Thread_Clear_state().
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

States_Control _Thread_Clear_state_locked(
  Thread_Control *the_thread,
  States_Control  state
)
{
  States_Control previous_state;

  _Assert( state != 0 );
  _Assert( _Thread_State_is_owner( the_thread ) );

  previous_state = the_thread->current_state;

  if ( ( previous_state & state ) != 0 ) {
    States_Control next_state;

    next_state = _States_Clear( state, previous_state );
    the_thread->current_state = next_state;

    if ( _States_Is_ready( next_state ) ) {
      _Scheduler_Unblock( the_thread );
    }
  }

  return previous_state;
}

States_Control _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
)
{
  ISR_lock_Context lock_context;
  States_Control   previous_state;

  _Thread_State_acquire( the_thread, &lock_context );
  previous_state = _Thread_Clear_state_locked( the_thread, state );
  _Thread_State_release( the_thread, &lock_context );

  return previous_state;
}
