/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIProfiling
 *
 * @brief This source file contains the implementation of
 *   rtems_profiling_iterate().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#include <rtems/profiling.h>
#include <rtems/counter.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smplock.h>
#include <rtems.h>

#include <string.h>

static void per_cpu_stats_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg,
  rtems_profiling_data *data
)
{
#ifdef RTEMS_PROFILING
  uint32_t n = rtems_scheduler_get_processor_maximum();
  uint32_t i;

  memset(data, 0, sizeof(*data));
  data->header.type = RTEMS_PROFILING_PER_CPU;
  for (i = 0; i < n; ++i) {
    const Per_CPU_Control *per_cpu = _Per_CPU_Get_by_index(i);
    const Per_CPU_Stats *stats = &per_cpu->Stats;
    rtems_profiling_per_cpu *per_cpu_data = &data->per_cpu;

    per_cpu_data->processor_index = i;

    per_cpu_data->max_thread_dispatch_disabled_time =
      rtems_counter_ticks_to_nanoseconds(
        stats->max_thread_dispatch_disabled_time
      );

    per_cpu_data->max_interrupt_time =
      rtems_counter_ticks_to_nanoseconds(stats->max_interrupt_time);

    per_cpu_data->max_interrupt_delay =
      rtems_counter_ticks_to_nanoseconds(stats->max_interrupt_delay);

    per_cpu_data->thread_dispatch_disabled_count =
      stats->thread_dispatch_disabled_count;

    per_cpu_data->total_thread_dispatch_disabled_time =
      rtems_counter_ticks_to_nanoseconds(
        stats->total_thread_dispatch_disabled_time
      );

    per_cpu_data->interrupt_count = stats->interrupt_count;

    per_cpu_data->total_interrupt_time =
      rtems_counter_ticks_to_nanoseconds(
        stats->total_interrupt_time
      );

    (*visitor)(visitor_arg, data);
  }
#else
  (void) visitor;
  (void) visitor_arg;
  (void) data;
#endif
}

#if defined(RTEMS_PROFILING) && defined(RTEMS_SMP)
RTEMS_STATIC_ASSERT(
  RTEMS_PROFILING_SMP_LOCK_CONTENTION_COUNTS
    == SMP_LOCK_STATS_CONTENTION_COUNTS,
  smp_lock_contention_counts
);
#endif

static void smp_lock_stats_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg,
  rtems_profiling_data *data
)
{
#if defined(RTEMS_PROFILING) && defined(RTEMS_SMP)
  SMP_lock_Stats_iteration_context iteration_context;
  SMP_lock_Stats snapshot;
  char name[64];

  memset(data, 0, sizeof(*data));
  data->header.type = RTEMS_PROFILING_SMP_LOCK;

  _SMP_lock_Stats_iteration_start(&iteration_context);
  while (
    _SMP_lock_Stats_iteration_next(
      &iteration_context,
      &snapshot,
      &name[0],
      sizeof(name)
    )
  ) {
    rtems_profiling_smp_lock *smp_lock_data = &data->smp_lock;

    smp_lock_data->name = name;
    smp_lock_data->max_acquire_time =
      rtems_counter_ticks_to_nanoseconds(snapshot.max_acquire_time);
    smp_lock_data->max_section_time =
      rtems_counter_ticks_to_nanoseconds(snapshot.max_section_time);
    smp_lock_data->usage_count = snapshot.usage_count;
    smp_lock_data->total_acquire_time =
      rtems_counter_ticks_to_nanoseconds(snapshot.total_acquire_time);
    smp_lock_data->total_section_time =
      rtems_counter_ticks_to_nanoseconds(snapshot.total_section_time);

    memcpy(
      &smp_lock_data->contention_counts[0],
      &snapshot.contention_counts[0],
      sizeof(smp_lock_data->contention_counts)
    );

    (*visitor)(visitor_arg, data);
  }
  _SMP_lock_Stats_iteration_stop(&iteration_context);
#else
  (void) visitor;
  (void) visitor_arg;
  (void) data;
#endif
}

void rtems_profiling_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg
)
{
  rtems_profiling_data data;

  per_cpu_stats_iterate(visitor, visitor_arg, &data);
  smp_lock_stats_iterate(visitor, visitor_arg, &data);
}
