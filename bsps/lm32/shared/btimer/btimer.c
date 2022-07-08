/* SPDX-License-Identifier: BSD-2-Clause */

/*  timer.c
 *
 *  This file manages the benchmark timer used by the RTEMS Timing
 *  Test Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().
 *  benchmark_timer_read() usually returns the number of microseconds
 *  since benchmark_timer_initialize() exitted.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <rtems/bspIo.h>
#include "../include/system_conf.h"
#include "../../shared/clock/clock.h"

static inline int timerread(unsigned int reg)
{
#if ON_SIMULATOR && defined(TIMER0_BASE_ADDRESS)
  return *((int*)(TIMER0_BASE_ADDRESS + reg));
#elif defined(TIMER1_BASE_ADDRESS)
  return *((int*)(TIMER1_BASE_ADDRESS + reg));
#else
#warning "Benchmarking timer not available!"
  return 0;
#endif
}

static inline void timerwrite(unsigned int reg, int value)
{
#if ON_SIMULATOR && defined(TIMER0_BASE_ADDRESS)
  *((int*)(TIMER0_BASE_ADDRESS + reg)) = value;
#elif defined(TIMER1_BASE_ADDRESS)
  *((int*)(TIMER1_BASE_ADDRESS + reg)) = value;
#endif
}

bool benchmark_timer_find_average_overhead;

void benchmark_timer_initialize( void )
{
  /* Set timer period */
  timerwrite(LM32_CLOCK_PERIOD, 0xffffffff);
  /* Stop timer */
  timerwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_STOP);
  /* Clear status register */
  timerwrite(LM32_CLOCK_SR, 0);
  /* Start timer */
  timerwrite(LM32_CLOCK_CR, LM32_CLOCK_CR_START);
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

#define AVG_OVERHEAD      4  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       4  /* Don't trust a clicks value lower than this */

benchmark_timer_t benchmark_timer_read( void )
{
  uint32_t ticks;
  uint32_t total;
  uint32_t status;

  ticks = 0xffffffff - timerread(LM32_CLOCK_SNAPSHOT);
  status = timerread(LM32_CLOCK_SR);
  if (status & LM32_CLOCK_SR_TO)
    printk("Timer overflow!\n");

  total = ticks / (CPU_FREQUENCY / 1000000);

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in XXX microsecond units */
  else
    {
      if ( total < LEAST_VALID )
	return 0;            /* below timer resolution */
      /*
       *  Somehow convert total into microseconds
       */
      return (total - AVG_OVERHEAD);
    }
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
