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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include "../include/system_conf.h"
#include "../../shared/clock/clock.h"

bool benchmark_timer_find_average_overhead;

void benchmark_timer_initialize(void)
{
  MM_WRITE(MM_TIMER1_COMPARE, 0xffffffff);
  MM_WRITE(MM_TIMER1_COUNTER, 0);
  MM_WRITE(MM_TIMER1_CONTROL, TIMER_ENABLE);
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

uint32_t benchmark_timer_read(void)
{
  uint32_t ticks;
  uint32_t total;

  ticks = MM_READ(MM_TIMER1_COUNTER);
  if (ticks == 0xffffffff)
    printk("Timer overflow!\n");

  total = ticks / (MM_READ(MM_FREQUENCY) / 1000000);

  if (benchmark_timer_find_average_overhead)
    return total;
  else
  {
    if (total < LEAST_VALID)
      return 0;

    return (total - AVG_OVERHEAD);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
