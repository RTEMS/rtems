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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/profiling.h>
#include <rtems/counter.h>
#include <rtems.h>

#include <string.h>

static void per_cpu_stats_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg,
  rtems_profiling_data *data
)
{
#ifdef RTEMS_PROFILING
  uint32_t n = rtems_smp_get_processor_count();
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

void rtems_profiling_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg
)
{
  rtems_profiling_data data;

  per_cpu_stats_iterate(visitor, visitor_arg, &data);
}
