/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
