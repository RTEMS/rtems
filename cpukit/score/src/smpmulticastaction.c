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
#include <rtems/score/threaddispatch.h>

typedef struct Per_CPU_Job Per_CPU_Job;

typedef struct Per_CPU_Jobs Per_CPU_Jobs;

/**
 * @brief A per-processor job.
 */
struct Per_CPU_Job {
  union {
    /**
     * @brief The next job in the corresponding per-processor job list.
     */
    Per_CPU_Job *next;

    /**
     * @brief Indication if the job is done.
     *
     * A job is done if this member has the value one.  This assumes that one
     * is not a valid pointer value.
     */
    Atomic_Ulong done;
  };

  /**
   * @brief Back pointer to the jobs to get the handler and argument.
   */
  Per_CPU_Jobs *jobs;
};

/**
 * @brief A collection of jobs, one for each processor.
 */
struct Per_CPU_Jobs {
  /**
   * @brief The job handler.
   */
  SMP_Action_handler handler;

  /**
   * @brief The job handler argument.
   */
  void *arg;

  /**
   * @brief One job for each potential processor.
   */
  Per_CPU_Job Jobs[ CPU_MAXIMUM_PROCESSORS ];
};

void _Per_CPU_Perform_jobs( Per_CPU_Control *cpu )
{
  ISR_lock_Context  lock_context;
  Per_CPU_Job      *job;

  _ISR_lock_ISR_disable( &lock_context );
  _Per_CPU_Acquire( cpu, &lock_context );

  while ( ( job = cpu->Jobs.head ) != NULL ) {
    Per_CPU_Jobs *jobs;

    cpu->Jobs.head = job->next;
    _Per_CPU_Release( cpu, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );

    jobs = job->jobs;
    ( *jobs->handler )( jobs->arg );
    _Atomic_Store_ulong( &job->done, 1, ATOMIC_ORDER_RELEASE );

     _ISR_lock_ISR_disable( &lock_context );
    _Per_CPU_Acquire( cpu, &lock_context );
  }

  _Per_CPU_Release( cpu, &lock_context );
  _ISR_lock_ISR_enable( &lock_context );
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

static void _SMP_Issue_action_jobs(
  const Processor_mask *targets,
  Per_CPU_Jobs         *jobs,
  uint32_t              cpu_max
)
{
  uint32_t cpu_index;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    if ( _Processor_mask_Is_set( targets, cpu_index ) ) {
      ISR_lock_Context  lock_context;
      Per_CPU_Job      *job;
      Per_CPU_Control  *cpu;

      job = &jobs->Jobs[ cpu_index ];
      _Atomic_Store_ulong( &job->done, 0, ATOMIC_ORDER_RELAXED );
      _Assert( job->next == NULL );
      job->jobs = jobs;

      cpu = _Per_CPU_Get_by_index( cpu_index );
      _ISR_lock_ISR_disable( &lock_context );
      _Per_CPU_Acquire( cpu, &lock_context );

      if ( cpu->Jobs.head == NULL ) {
        cpu->Jobs.head = job;
      } else {
        *cpu->Jobs.tail = job;
      }

      cpu->Jobs.tail = &job->next;

      _Per_CPU_Release( cpu, &lock_context );
      _ISR_lock_ISR_enable( &lock_context );
      _SMP_Send_message( cpu_index, SMP_MESSAGE_PERFORM_JOBS );
    }
  }
}

static void _SMP_Wait_for_action_jobs(
  const Processor_mask *targets,
  const Per_CPU_Jobs   *jobs,
  uint32_t              cpu_max,
  Per_CPU_Control      *cpu_self
)
{
  uint32_t cpu_index;

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    if ( _Processor_mask_Is_set( targets, cpu_index ) ) {
      const Per_CPU_Job *job;
      Per_CPU_Control   *cpu;

      job = &jobs->Jobs[ cpu_index ];
      cpu = _Per_CPU_Get_by_index( cpu_index );

      while ( _Atomic_Load_ulong( &job->done, ATOMIC_ORDER_ACQUIRE ) == 0 ) {
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
            _Per_CPU_Try_perform_jobs( cpu_self );
            break;
          default:
            _SMP_Fatal( SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS );
            break;
        }
      }
    }
  }
}

void _SMP_Multicast_action(
  const Processor_mask *targets,
  SMP_Action_handler    handler,
  void                 *arg
)
{
  Per_CPU_Jobs     jobs;
  uint32_t         cpu_max;
  Per_CPU_Control *cpu_self;
  uint32_t         isr_level;

  cpu_max = _SMP_Get_processor_maximum();
  _Assert( cpu_max <= CPU_MAXIMUM_PROCESSORS );

  if ( targets == NULL ) {
    targets = _SMP_Get_online_processors();
  }

  jobs.handler = handler;
  jobs.arg = arg;
  isr_level = _ISR_Get_level();

  if ( isr_level == 0 ) {
    cpu_self = _Thread_Dispatch_disable();
  } else {
    cpu_self = _Per_CPU_Get();
  }

  _SMP_Issue_action_jobs( targets, &jobs, cpu_max );
  _SMP_Wait_for_action_jobs( targets, &jobs, cpu_max, cpu_self );

  if ( isr_level == 0 ) {
    _Thread_Dispatch_enable( cpu_self );
  }
}
