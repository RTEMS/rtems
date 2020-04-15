/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
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
  delta = _CPU_Counter_difference(
    interrupt_exit_instant,
    interrupt_entry_instant
  );
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
