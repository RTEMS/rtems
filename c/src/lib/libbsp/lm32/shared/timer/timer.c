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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
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

uint32_t benchmark_timer_read( void )
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
