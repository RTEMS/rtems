/*
 * Cogent CSB336 Timer driver
 *
 * This uses timer 2 for timing measurments.
 *
 * Copyright (c) 2004 Cogent Computer Systems
 *        Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Notes:
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().  benchmark_timer_read() usually returns
 *  the number of microseconds since benchmark_timer_initialize() exitted.
 *
 *  It is important that the timer start/stop overhead be determined
 *  when porting or modifying this code.
*/

#include <rtems.h>
#include <bsp.h>
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

int benchmark_timer_read( void )
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

