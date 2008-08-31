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
#include <mc9328mxl.h>

uint32_t g_start;
uint32_t g_freq;

rtems_boolean benchmark_timerfind_average_overhead;

    
/*
 * Set up Timer 1
 */
void benchmark_timerinitialize( void )
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

  if ( benchmark_timerfind_average_overhead == 1 ) {
    return (int) total; 
  } else if ( total < LEAST_VALID ) {
      return 0;       
  }
  /*
   *  Somehow convert total into microseconds
   */

  return (total - AVG_OVERHEAD);
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

