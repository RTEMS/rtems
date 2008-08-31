/*
 * RTL22xx board Timer driver
 *
 * This uses Timer1 for timing measurments.
 *  
 *  By Ray xu<rayx.cn@gmail.com>, modify form Mc9328mxl	RTEMS DSP
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 * Notes:
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timerinitialize() and benchmark_timerread().  benchmark_timerread() usually returns
 *  the number of microseconds since benchmark_timerinitialize() exitted.
 *
 *  It is important that the timer start/stop overhead be determined 
 *  when porting or modifying this code.
 *
 *  $Id$
*/

#include <rtems.h>
#include <bsp.h>
#include <lpc22xx.h>
#include "lpc_timer.h"
uint32_t g_start;
uint32_t g_freq;

rtems_boolean benchmark_timerfind_average_overhead;

    
/*
 * Set up Timer 1
 */
void benchmark_timerinitialize( void )
{
       g_freq = LPC22xx_Fpclk / 1000;
}

/*
 *  The following controls the behavior of benchmark_timerread().
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

int benchmark_timerread( void )
{
  return (T0TC/(LPC22xx_Fpclk/1000000));
  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */
}

/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

rtems_status_code benchmark_timerempty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void benchmark_timerdisable_subtracting_average_overhead(
  rtems_boolean find_flag
)
{
  benchmark_timerfind_average_overhead = find_flag;
}

