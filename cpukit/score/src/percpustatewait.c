/**
 * @file
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This source file contains the implementation of
 *   _Per_CPU_State_wait_for_non_initial_state().
 */

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

#include <rtems/score/percpu.h>
#include <rtems/counter.h>

bool _Per_CPU_State_wait_for_non_initial_state(
  uint32_t cpu_index,
  uint32_t timeout_in_ns
)
{
  const Per_CPU_Control *cpu;
  Per_CPU_State          state;

  cpu = _Per_CPU_Get_by_index( cpu_index );
  state = _Per_CPU_Get_state( cpu );

  if ( timeout_in_ns > 0 ) {
    rtems_counter_ticks ticks =
      rtems_counter_nanoseconds_to_ticks( timeout_in_ns );
    rtems_counter_ticks a = rtems_counter_read();
    rtems_counter_ticks delta = 0;

    while ( ticks > delta && state == PER_CPU_STATE_INITIAL ) {
      rtems_counter_ticks b;

      state = _Per_CPU_Get_state( cpu );

      ticks -= delta;
      b = rtems_counter_read();
      delta = rtems_counter_difference( b, a );
      a = b;
    }
  } else {
    while ( state == PER_CPU_STATE_INITIAL ) {
      state = _Per_CPU_Get_state( cpu );
    }
  }

  return state != PER_CPU_STATE_INITIAL;
}
