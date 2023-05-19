/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMP
 *
 * @brief This source file contains the implementation of
 *   _SMP_Multicast_action().
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

      _Per_CPU_Submit_job( cpu, job );
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
