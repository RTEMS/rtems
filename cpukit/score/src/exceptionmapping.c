/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreExceptionMapping
 *
 * @brief AArch64 machine exception to POSIX signal mapping.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <pthread.h>
#include <signal.h>
#include <rtems/score/exception.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadimpl.h>

static _Thread_local int           raise_signal;
static _Thread_local Thread_Action _Exception_Raise_signal_action;

static void _Exception_Raise_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
)
{
  _Thread_State_release( executing, lock_context );
  raise( raise_signal );
  _Thread_State_acquire( executing, lock_context );
}

/*
 * Exception handler. Map the exception class to SIGFPE, SIGSEGV
 * or SIGILL for Ada or other runtimes.
 */
void _Exception_Raise_signal(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
)
{
  CPU_Exception_frame *ef;
  Per_CPU_Control     *cpu_self = _Per_CPU_Get();
  bool                 system_up;

  if ( source != RTEMS_FATAL_SOURCE_EXCEPTION ) {
    return;
  }

  /* If the CPU isn't UP yet, there isn't anything to send a signal to */
#ifdef RTEMS_SMP
  system_up = ( _Per_CPU_Get_state( cpu_self ) == PER_CPU_STATE_UP );
#else
  system_up = ( _System_state_Get() == SYSTEM_STATE_UP );
#endif

  if ( !system_up ) {
    return;
  }

  ef = (rtems_exception_frame *) code;
  raise_signal = _CPU_Exception_frame_get_signal( ef );

  if ( raise_signal < 0 ) {
    return;
  }

  _Thread_Add_post_switch_action(
    _Per_CPU_Get_executing( cpu_self ),
    &_Exception_Raise_signal_action,
    _Exception_Raise_handler
  );

  /* Disable thread dispatch so that dispatch can occur */
  _CPU_Exception_disable_thread_dispatch();

  /* Perform dispatch and resume execution */
  _CPU_Exception_dispatch_and_resume( ef );
}
