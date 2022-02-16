/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Start().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/userextimpl.h>

Status_Control _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
)
{
  Per_CPU_Control *cpu_self;

  _Thread_State_acquire_critical( the_thread, lock_context );

  if ( !_States_Is_dormant( the_thread->current_state ) ) {
    _Thread_State_release( the_thread, lock_context );
    return STATUS_INCORRECT_STATE;
  }

  the_thread->Start.Entry = *entry;
  _Thread_Load_environment( the_thread );
  _Thread_Clear_state_locked( the_thread, STATES_ALL_SET );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Thread_State_release( the_thread, lock_context );

  _User_extensions_Thread_start( the_thread );

  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}
