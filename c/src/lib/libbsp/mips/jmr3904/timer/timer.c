/* 
 *  This file implements a benchmark timer using a TX39 timer.
 *
 *  NOTE: On the simulator, the count directly reflects instructions.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <assert.h>

#include <bsp.h>

rtems_boolean Timer_driver_Find_average_overhead;

void Timer_initialize()
{
  /*
   *  Programming the compare register as the maximum value should let
   *  it run long enough and accurate enough not to require an interrupt.
   *  but if it ever does generate an interrupt, we will simply fault.
   *
   *  NOTE:  This is similar to the clock driver initialization
   *         with the exception that the divider is disabled and
   *         the compare register is set to the maximum value.
   */

  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_TCR,   0x20 );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_CCDR, 0x3 );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_TRR, 0x0 );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_CPRA, 0xFFFFFFFF );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_TISR, 0x00 );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_ITMR, 0x0001 );
  TX3904_TIMER_WRITE( TX3904_TIMER1_BASE, TX3904_TIMER_TCR,   0xe0 );
}

#define AVG_OVERHEAD      0  /* It typically takes N instructions */
                             /*     to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* tx39 simulator can count instructions. :) */

int Read_timer()
{
  rtems_unsigned32  total;

  total = TX3904_TIMER_READ( TX3904_TIMER1_BASE, TX3904_TIMER_TRR );

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

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
