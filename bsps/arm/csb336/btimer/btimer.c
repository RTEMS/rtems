/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Cogent CSB336 Timer driver
 *
 * This uses timer 2 for timing measurments.
 */

/*
 * Copyright (C) 2004 Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <mc9328mxl.h>

uint32_t g_start;
uint32_t g_freq;

bool benchmark_timer_find_average_overhead;


/*
 * Set up Timer 1
 */
void benchmark_timer_initialize( void )
{
    MC9328MXL_TMR2_TCTL = (MC9328MXL_TMR_TCTL_CLKSRC_PCLK1 |
                            MC9328MXL_TMR_TCTL_FRR |
                            MC9328MXL_TMR_TCTL_TEN);
    /* set prescaler to 1 (register value + 1) */ \
    MC9328MXL_TMR2_TPRER = 0;

    /* get freq of counter in KHz */
    g_freq = get_perclk1_freq() / 1000;

    g_start =  MC9328MXL_TMR2_TCN;
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

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

benchmark_timer_t benchmark_timer_read( void )
{
  uint32_t t;
  unsigned long long total;

  t =  MC9328MXL_TMR2_TCN;
  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = (t - g_start);

  /* convert to nanoseconds */
  total = (total * 1000)/ g_freq;

  if ( benchmark_timer_find_average_overhead == 1 ) {
    return (int) total;
  } else if ( total < LEAST_VALID ) {
      return 0;
  }
  /*
   *  Somehow convert total into microseconds
   */

  return (total - AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}

