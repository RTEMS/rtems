/*
 *  This file implements a benchmark timer using a MONGOOSE-V timer.
 *
 *  COPYRIGHT (c) 1989-2001.
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

#if defined(USE_TIMER2_FOR_CLOCK)
#define TIMER_BASE   MONGOOSEV_TIMER1_BASE
#define TIMER_VECTOR MONGOOSEV_IRQ_TIMER1
#else
#define TIMER_BASE   MONGOOSEV_TIMER2_BASE
#define TIMER_VECTOR MONGOOSEV_IRQ_TIMER2
#endif

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

   MONGOOSEV_WRITE_REGISTER( TIMER_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0);

   MONGOOSEV_WRITE_REGISTER( TIMER_BASE,
			     MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER,
			     0xffffffff );

   MONGOOSEV_WRITE_REGISTER( TIMER_BASE,
			     MONGOOSEV_TIMER_CONTROL_REGISTER,
			     MONGOOSEV_TIMER_CONTROL_COUNTER_ENABLE );

}

#define AVG_OVERHEAD      0  /* It typically takes N instructions */
                             /*     to start/stop the timer. */

#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* mongoose-v can count cycles. :) */
#include <rtems/bspIo.h>

uint32_t benchmark_timer_read(void)
{
  uint32_t          clicks;
  uint32_t          total;
  uint32_t          tcr;

  clicks = MONGOOSEV_READ_REGISTER( TIMER_BASE,
				    MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER );
  total = 0xffffffff - clicks;

  tcr = MONGOOSEV_READ_REGISTER( TIMER_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER );

  MONGOOSEV_WRITE_REGISTER( TIMER_BASE,
			    MONGOOSEV_TIMER_CONTROL_REGISTER,
			    0 );

  if ( tcr & MONGOOSEV_TIMER_CONTROL_TIMEOUT )
    printk( "MG5 timer overran\n" );

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in cycle units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total - AVG_OVERHEAD) / CPU_CLOCK_RATE_MHZ;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}

/* eof */
