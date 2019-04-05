/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
