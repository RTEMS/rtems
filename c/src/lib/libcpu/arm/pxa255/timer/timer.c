/*
 * PXA255 timer by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 * Notes:
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
 *
 *  It is important that the timer start/stop overhead be determined
 *  when porting or modifying this code.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <pxa255.h>

uint32_t tstart;
static uint32_t tick_time;
bool benchmark_timer_find_average_overhead;

bool benchmark_timer_is_initialized = false;

/*
 * Use the timer count register to measure.
 * The frequency of it is 3.4864MHZ
 * The longest period we are able to capture is 4G/3.4864MHZ
 */
void benchmark_timer_initialize(void)
{
  tick_time = XSCALE_OS_TIMER_TCR;
}

/*
 *  The following controls the behavior of Read_timer().
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

int benchmark_timer_read(void)
{

  uint32_t total;
  total = XSCALE_OS_TIMER_TCR;
  if(total>=tick_time)
    total -= tick_time;
  else
    total += 0xffffffff - tick_time; /*Round up but not overflow*/

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /*Counter cycles*/

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
