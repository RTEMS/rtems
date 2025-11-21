/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

#include <bspopts.h>
#include <bsp/microblaze-fdt-support.h>
#include <bsp/microblaze-timer.h>

#include <rtems.h>
#include <rtems/btimer.h>

static volatile Microblaze_Timer *mblaze_timer;
bool                              benchmark_timer_find_average_overhead;

void benchmark_timer_initialize ( void )
{
  mblaze_timer = (volatile Microblaze_Timer *) try_get_prop_from_device_tree (
    "xlnx,xps-timer-1.00.a", "reg", BSP_MICROBLAZE_FPGA_TIMER_BASE );

  /* Set load register to 0 */
  mblaze_timer->tlr0 = 0;
  /* Reset the timer and interrupt */
  mblaze_timer->tcsr0 =
    MICROBLAZE_TIMER_TCSR0_T0INT | MICROBLAZE_TIMER_TCSR0_LOAD0;
  /* Release the reset */
  mblaze_timer->tcsr0 = 0;
  /*
   * Enable interrupt, auto reload mode, external interrupt signal,
   * and down counter
   */
  mblaze_timer->tcsr0 =
    MICROBLAZE_TIMER_TCSR0_ARHT0 | MICROBLAZE_TIMER_TCSR0_ENIT0 |
    MICROBLAZE_TIMER_TCSR0_GENT0 | MICROBLAZE_TIMER_TCSR0_UDT0;

  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick ();
  uint32_t counter_frequency_in_hz =
    try_get_prop_from_device_tree ( "xlnx,xps-timer-1.00.a",
                                    "clock-frequency",
                                    BSP_MICROBLAZE_FPGA_TIMER_FREQUENCY );
  uint32_t counter_ticks_per_clock_tick =
    ( counter_frequency_in_hz * us_per_tick ) / 1000000;

  /* Set a reset value for the timer counter */
  mblaze_timer->tlr0 = counter_ticks_per_clock_tick;
  uint32_t control_status_reg = mblaze_timer->tcsr0;
  /* Load the reset value into the counter register */
  mblaze_timer->tcsr0 = MICROBLAZE_TIMER_TCSR0_LOAD0;
  /* Enable the timer */
  mblaze_timer->tcsr0 = control_status_reg | MICROBLAZE_TIMER_TCSR0_ENT0;
}

/*
 *  The following controls the behavior of benchmark_timer_read().
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD 0 /* It typically takes X.X microseconds */
                       /* (Y countdowns) to start/stop the timer. */
                       /* This value is in microseconds. */
#define LEAST_VALID 1  /* Don't trust a clicks value lower than this */

benchmark_timer_t benchmark_timer_read ( void )
{
  uint32_t ticks;
  uint32_t total;

  ticks = 1000000 - mblaze_timer->tcr0;

  total = ticks / ( BSP_MICROBLAZE_FPGA_TIMER_FREQUENCY / 1000000 );

  if ( benchmark_timer_find_average_overhead == true )
    return total; /* in XXX microsecond units */
  else
  {
    if ( total < LEAST_VALID )
      return 0; /* below timer resolution */

    return ( total - AVG_OVERHEAD );
  }
}

void benchmark_timer_disable_subtracting_average_overhead ( bool find_flag )
{
  benchmark_timer_find_average_overhead = find_flag;
}
