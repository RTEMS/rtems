/*  Timer for Blackfin
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
 *  
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <rtems.h>
#include <bsp.h>


uint32_t         Timer_interrupts;
rtems_boolean Timer_driver_Find_average_overhead;

/*
 * Timer_initialize
 * 
 * Blackfin processor has a counter for clock cycles.
 */
void Timer_initialize( void )
{

  /*reset counters*/
  asm ("R2 = 0;");
  asm ("CYCLES = R2;");
  asm ("CYCLES2 = R2;");
  /*start counters*/
  asm ("R2 = SYSCFG;");
  asm ("BITSET(R2,1);");
  asm ("SYSCFG = R2");
 
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
  uint32_t          clicks;
  uint32_t          total;
  register uint32_t cycles asm ("R2");

  /* stop counter */ 
  asm("R2 = SYSCFG;");
  asm("BITCLR(R2,1);");
  asm("SYSCFG = R2;");
  asm("R2 = CYCLES;");


  clicks = cycles; /* Clock cycles */

  /* converting to microseconds */
  total = clicks / (CCLK/1000000); 

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */
  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  Somehow convert total into microseconds
   */
      return (total - AVG_OVERHEAD);
    }
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
