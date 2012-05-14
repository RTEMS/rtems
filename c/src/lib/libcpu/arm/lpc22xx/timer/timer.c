/*
 * RTL22xx board Timer driver
 *
 * This uses Timer1 for timing measurments.
 *
 *  By Ray xu<rayx.cn@gmail.com>, modify form Mc9328mxl	RTEMS DSP
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
#include <lpc22xx.h>
#include "lpc_timer.h"
uint32_t g_start;
uint32_t g_freq;

bool benchmark_timer_find_average_overhead;


/*
 * Set up Timer 1
 */
void benchmark_timer_initialize( void )
{
       g_freq = LPC22xx_Fpclk / 1000;
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
  return (T0TC/(LPC22xx_Fpclk/1000000));
  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}

