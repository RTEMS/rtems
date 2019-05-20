/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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

static void _Per_CPU_Try_perform_jobs( Per_CPU_Control *cpu_self )
{
  unsigned long message;

  message = _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED );

  if ( ( message & SMP_MESSAGE_PERFORM_JOBS ) != 0 ) {
    bool success;

    success = _Atomic_Compare_exchange_ulong(
      &cpu_self->message, &message,
      message & ~SMP_MESSAGE_PERFORM_JOBS, ATOMIC_ORDER_RELAXED,
      ATOMIC_ORDER_RELAXED
    );

    if ( success ) {
      _Per_CPU_Perform_jobs( cpu_self );
    }
  }
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
    switch ( cpu->state ) {
      case PER_CPU_STATE_INITIAL:
      case PER_CPU_STATE_READY_TO_START_MULTITASKING:
      case PER_CPU_STATE_REQUEST_START_MULTITASKING:
        _CPU_SMP_Processor_event_broadcast();
        /* Fall through */
      case PER_CPU_STATE_UP:
        /*
         * Calling this function with the current processor is intentional.
         * We have to perform our own jobs here in case inter-processor
         * interrupts are not working.
         */
        _Per_CPU_Try_perform_jobs( _Per_CPU_Get() );
        break;
      default:
        _SMP_Fatal( SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS );
        break;
    }
  }
}

typedef struct {
  Per_CPU_Job_context Context;
  Per_CPU_Job         Jobs[ CPU_MAXIMUM_PROCESSORS ];
} SMP_Multicast_jobs;

static void _SMP_Issue_action_jobs(
  const Processor_mask *targets,
  SMP_Multicast_jobs   *jobs,
  uint32_t              cpu_max
)
{
  uint32_t cpu_index;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    if ( _Processor_mask_Is_set( targets, cpu_index ) ) {
      Per_CPU_Job     *job;
      Per_CPU_Control *cpu;

      job = &jobs->Jobs[ cpu_index ];
      job->context = &jobs->Context;
      cpu = _Per_CPU_Get_by_index( cpu_index );

      _Per_CPU_Add_job( cpu, job );
      _SMP_Send_message( cpu_index, SMP_MESSAGE_PERFORM_JOBS );
    }
  }
}

static void _SMP_Wait_for_action_jobs(
  const Processor_mask     *targets,
  const SMP_Multicast_jobs *jobs,
  uint32_t                  cpu_max
)
{
  uint32_t cpu_index;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    if ( _Processor_mask_Is_set( targets, cpu_index ) ) {
      const Per_CPU_Control *cpu;
      const Per_CPU_Job     *job;

      cpu = _Per_CPU_Get_by_index( cpu_index );
      job = &jobs->Jobs[ cpu_index ];
      _Per_CPU_Wait_for_job( cpu, job );
    }
  }
}

void _SMP_Multicast_action(
  const Processor_mask *targets,
  SMP_Action_handler    handler,
  void                 *arg
)
{
  SMP_Multicast_jobs jobs;
  uint32_t           cpu_max;

  cpu_max = _SMP_Get_processor_maximum();
  _Assert( cpu_max <= RTEMS_ARRAY_SIZE( jobs.Jobs ) );

  jobs.Context.handler = handler;
  jobs.Context.arg = arg;

  _SMP_Issue_action_jobs( targets, &jobs, cpu_max );
  _SMP_Wait_for_action_jobs( targets, &jobs, cpu_max );
}

void _SMP_Broadcast_action(
  SMP_Action_handler  handler,
  void               *arg
)
{
  _SMP_Multicast_action( _SMP_Get_online_processors(), handler, arg );
}

void _SMP_Othercast_action(
  SMP_Action_handler  handler,
  void               *arg
)
{
  Processor_mask targets;

  _Processor_mask_Assign( &targets, _SMP_Get_online_processors() );
  _Processor_mask_Clear( &targets, _SMP_Get_current_processor() );
  _SMP_Multicast_action( &targets, handler, arg );
}

static void _SMP_Do_nothing_action( void *arg )
{
  /* Do nothing */
}

void _SMP_Synchronize( void )
{
  _SMP_Othercast_action( _SMP_Do_nothing_action, NULL );
}
