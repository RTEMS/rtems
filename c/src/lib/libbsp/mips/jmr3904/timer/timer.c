/*
 *  This file implements a benchmark timer using a TX39 timer.
 *
 *  NOTE: On the simulator, the count directly reflects instructions.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;

void benchmark_timer_initialize(void)
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

uint32_t benchmark_timer_read(void)
{
  uint32_t          total;

  total = TX3904_TIMER_READ( TX3904_TIMER1_BASE, TX3904_TIMER_TRR );

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
