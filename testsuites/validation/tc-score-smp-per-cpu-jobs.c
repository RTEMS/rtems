/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSmpValPerCpuJobs
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreSmpValPerCpuJobs spec:/score/smp/val/per-cpu-jobs
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests the processing order of per-processor jobs.
 *
 * This test case performs the following actions:
 *
 * - Issue two jobs on the current processor with interrupts disabled.  Wait
 *   for completion of the second job.
 *
 *   - Check that the first job was processed firstly.
 *
 *   - Check that the second job was processed secondly.
 *
 * @{
 */

static Atomic_Uint job_counter;

static void Increment( void *arg )
{
  unsigned int *value;

  value = (unsigned int *) arg;
  *value =
    _Atomic_Fetch_add_uint( &job_counter, 1, ATOMIC_ORDER_RELAXED ) + 1;
}

static unsigned int counter_0;

static const Per_CPU_Job_context job_context_0 = {
  .handler = Increment,
  .arg = &counter_0
};

Per_CPU_Job job_0 = {
  .context = &job_context_0
};

static unsigned int counter_1;

static const Per_CPU_Job_context job_context_1 = {
  .handler = Increment,
  .arg = &counter_1
};

Per_CPU_Job job_1 = {
  .context = &job_context_1,
};

/**
 * @brief Issue two jobs on the current processor with interrupts disabled.
 *   Wait for completion of the second job.
 */
static void ScoreSmpValPerCpuJobs_Action_0( void )
{
  rtems_interrupt_level level;
  Per_CPU_Control      *cpu;

  rtems_interrupt_local_disable(level);
  cpu = _Per_CPU_Get();
  _Per_CPU_Add_job( cpu, &job_0 );
  _Per_CPU_Submit_job( cpu, &job_1 );
  rtems_interrupt_local_enable(level);

  _Per_CPU_Wait_for_job( cpu, &job_1 );

  /*
   * Check that the first job was processed firstly.
   */
  T_step_eq_int( 0, counter_0, 1 );

  /*
   * Check that the second job was processed secondly.
   */
  T_step_eq_int( 1, counter_1, 2 );
}

/**
 * @fn void T_case_body_ScoreSmpValPerCpuJobs( void )
 */
T_TEST_CASE( ScoreSmpValPerCpuJobs )
{
  T_plan( 2 );

  ScoreSmpValPerCpuJobs_Action_0();
}

/** @} */
