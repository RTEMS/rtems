/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Start_multitasking().
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/score/smpimpl.h>

void _Thread_Start_multitasking( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  Thread_Control  *heir;

#if defined(RTEMS_SMP)
  _Per_CPU_Set_state( cpu_self, PER_CPU_STATE_UP );
  _SMP_Try_to_process_message( cpu_self, SMP_MESSAGE_FORCE_PROCESSING );
#endif

  heir = _Thread_Get_heir_and_make_it_executing( cpu_self );

  _Profiling_Thread_dispatch_disable( cpu_self, 0 );

#if defined(RTEMS_SMP)
  _CPU_SMP_Prepare_start_multitasking();
#endif

#if defined(_CPU_Start_multitasking)
  _CPU_Start_multitasking( &heir->Registers );
#elif defined(RTEMS_SMP)
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
#error "The CPU port has to provide _CPU_Start_multitasking()"
#endif
  {
    Context_Control trash;

    /*
     * Mark the trash context as executing to not confuse the
     * _CPU_Context_switch().  On SMP configurations the context switch
     * contains a special hand over section to atomically switch from the
     * executing to the currently selected heir thread.
     */
    _CPU_Context_Set_is_executing( &trash, true );
    _CPU_Context_switch_no_return( &trash, &heir->Registers );
    RTEMS_UNREACHABLE();
  }
#else
  _CPU_Context_Restart_self( &heir->Registers );
#endif
}
