/*
 * RTL22xx board Timer driver
 *
 * This uses Timer1 for timing measurments.
 *  
 *  By Ray xu, modify form Mc9328mxl	RTEMS DSP
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  timer.c,v 1.1 2002/11/13 17:55:09 joel Exp
*/

#include <rtems.h>
#include <bsp.h>
#include <lpc22xx.h>
#include "lpc_timer.h"
uint32_t g_start;
uint32_t g_freq;

#ifndef __cplusplus
#define bool	int
#define true	1
#define false	0

#endif/* __cplusplus */

rtems_boolean Timer_driver_Find_average_overhead;

    
/*
 * Set up Timer 1
 */
void Timer_initialize( void )
{
	T1TCR &= 0; 	 /* disable and clear timer 0  */ 
       g_start = (T1TC/(LPC22xx_Fpclk/1000000));
       T1PC  = 0;            /* TC is incrementet on every pclk.*/ 
	T1MR0 = ((LPC22xx_Fpclk/1000* BSP_Configuration.microseconds_per_tick) / 1000); /* initialize the timer period and prescaler */  
       T1EMR = 0;  /*No external match*/ 
	T1TCR = 1; /*enable timer1*/ 
       g_freq = LPC22xx_Fpclk / 1000;
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

int Read_timer( void )
{
  uint32_t t;
  unsigned long long total;
  
  t = (T1TC/(LPC22xx_Fpclk/1000000));
  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts. currently it is not supported
   */

  total = (t - g_start);

  /* convert to nanoseconds */

  if ( Timer_driver_Find_average_overhead == 1 ) {
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

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}

