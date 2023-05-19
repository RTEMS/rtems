/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreProfiling
 *
 * @brief This source file contains the implementation of
 *   _Profiling_Outer_most_interrupt_entry_and_exit().
 */

/*
 * Copyright (C) 2014, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/profiling.h>
#include <rtems/score/assert.h>

void _Profiling_Outer_most_interrupt_entry_and_exit(
  Per_CPU_Control *cpu,
  CPU_Counter_ticks interrupt_entry_instant,
  CPU_Counter_ticks interrupt_exit_instant
)
{
#if defined(RTEMS_PROFILING)
  Per_CPU_Stats     *stats;
  CPU_Counter_ticks  delta;

  _Assert( cpu->isr_nest_level == 1 );

  stats = &cpu->Stats;
  delta = interrupt_exit_instant - interrupt_entry_instant;
  ++stats->interrupt_count;
  stats->total_interrupt_time += delta;

  if ( stats->max_interrupt_time < delta ) {
    stats->max_interrupt_time = delta;
  }

  if ( cpu->thread_dispatch_disable_level == 1 ) {
    stats->thread_dispatch_disabled_instant = interrupt_entry_instant;
  }
#else
  (void) cpu;
  (void) interrupt_entry_instant;
  (void) interrupt_exit_instant;
#endif
}
