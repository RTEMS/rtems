/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicTimecounter
 *
 * @brief This source file contains the implementation of
 *   rtems_timecounter_simple_install().
 */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#include <rtems/timecounter.h>

void rtems_timecounter_simple_install(
  rtems_timecounter_simple *tc,
  uint32_t                  frequency_in_hz,
  uint32_t                  counter_ticks_per_clock_tick,
  timecounter_get_t        *get_timecount
)
{
  uint32_t power_of_two = 1;
  uint32_t mask;
  uint64_t scaler;
  int i;

  for ( i = 0; i < 32; ++i ) {
    if ( power_of_two >= counter_ticks_per_clock_tick ) {
      break;
    }

    power_of_two *= 2;
  }

  mask = ( 2 * power_of_two ) - 1;
  scaler = ( (uint64_t) power_of_two << 32 ) / counter_ticks_per_clock_tick;

  tc->scaler = scaler;
  tc->real_interval = counter_ticks_per_clock_tick;
  tc->binary_interval = ( mask + 1 ) / 2;
  tc->tc.tc_get_timecount = get_timecount;
  tc->tc.tc_counter_mask = mask;
  tc->tc.tc_frequency = (uint32_t) ( ( frequency_in_hz * scaler ) >> 32 );
  tc->tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  _Timecounter_Install( &tc->tc );
}
