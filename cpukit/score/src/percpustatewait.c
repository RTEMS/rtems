/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This source file contains the implementation of
 *   _Per_CPU_State_wait_for_non_initial_state().
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
