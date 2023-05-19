/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This source file contains the implementation of _Per_CPU_Add_job(),
 *   _Per_CPU_Perform_jobs(), _Per_CPU_Submit_job(), and
 *   _Per_CPU_Wait_for_job().
 */

/*
 * Copyright (C) 2019 embedded brains GmbH & Co. KG
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

#include <rtems/score/smpimpl.h>
#include <rtems/score/assert.h>

#define _Per_CPU_Jobs_ISR_disable_and_acquire( cpu, lock_context ) \
  _ISR_lock_ISR_disable_and_acquire( &( cpu )->Jobs.Lock, lock_context )

#define _Per_CPU_Jobs_release_and_ISR_enable( cpu, lock_context ) \
  _ISR_lock_Release_and_ISR_enable( &( cpu )->Jobs.Lock, lock_context )

void _Per_CPU_Perform_jobs( Per_CPU_Control *cpu )
{
  ISR_lock_Context  lock_context;
  Per_CPU_Job      *job;

  _Per_CPU_Jobs_ISR_disable_and_acquire( cpu, &lock_context );
  job = cpu->Jobs.head;
  cpu->Jobs.head = NULL;
  _Per_CPU_Jobs_release_and_ISR_enable( cpu, &lock_context );

  while ( job != NULL ) {
    const Per_CPU_Job_context *context;
    Per_CPU_Job               *next;

    context = job->context;
    next = job->next;
    ( *context->handler )( context->arg );
    _Atomic_Store_ulong( &job->done, PER_CPU_JOB_DONE, ATOMIC_ORDER_RELEASE );

    job = next;
  }
}

void _Per_CPU_Add_job( Per_CPU_Control *cpu, Per_CPU_Job *job )
{
  ISR_lock_Context lock_context;

  _Assert( job->context != NULL && job->context->handler != NULL );

  _Atomic_Store_ulong( &job->done, 0, ATOMIC_ORDER_RELAXED );
  _Assert( job->next == NULL );

  _Per_CPU_Jobs_ISR_disable_and_acquire( cpu, &lock_context );

  if ( cpu->Jobs.head == NULL ) {
    cpu->Jobs.head = job;
  } else {
    *cpu->Jobs.tail = job;
  }

  cpu->Jobs.tail = &job->next;

  _Per_CPU_Jobs_release_and_ISR_enable( cpu, &lock_context );
}

void _Per_CPU_Submit_job( Per_CPU_Control *cpu, Per_CPU_Job *job )
{
  _Per_CPU_Add_job( cpu, job );
  _SMP_Send_message( cpu, SMP_MESSAGE_PERFORM_JOBS );
}

void _Per_CPU_Wait_for_job(
  const Per_CPU_Control *cpu,
  const Per_CPU_Job     *job
)
{
  while (
    _Atomic_Load_ulong( &job->done, ATOMIC_ORDER_ACQUIRE )
      != PER_CPU_JOB_DONE
  ) {
    Per_CPU_Control *cpu_self;

    switch ( _Per_CPU_Get_state( cpu ) ) {
      case PER_CPU_STATE_INITIAL:
      case PER_CPU_STATE_READY_TO_START_MULTITASKING:
      case PER_CPU_STATE_UP:
        /*
         * Calling this function with the current processor is intentional.  We
         * have to perform our own jobs here in case inter-processor interrupts
         * are not working.
         */
        cpu_self = _Per_CPU_Get();
        _SMP_Try_to_process_message(
          cpu_self,
          _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED )
        );
        break;
      default:
        _SMP_Fatal( SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS );
    }
  }
}
