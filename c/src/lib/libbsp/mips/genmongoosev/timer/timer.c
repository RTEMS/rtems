/* 
 *  This file implements a benchmark timer using a MONGOOSE-V timer.
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

#if defined(USE_TIMER2_FOR_CLOCK)
#define TIMER_BASE   MONGOOSEV_TIMER1_BASE
#define TIMER_VECTOR MONGOOSEV_IRQ_TIMER1
#else
#define TIMER_BASE   MONGOOSEV_TIMER2_BASE
#define TIMER_VECTOR MONGOOSEV_IRQ_TIMER2
#endif

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

  MONGOOSEV_WRITE_REGISTER(
    TIMER_BASE,
    MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER,
    0xffffffff
  );
  MONGOOSEV_WRITE_REGISTER(
    TIMER_BASE,
    MONGOOSEV_TIMER_CONTROL_REGISTER,
    MONGOOSEV_TIMER_CONTROL_COUNTER_ENABLE
  );
}

#define AVG_OVERHEAD      0  /* It typically takes N instructions */
                             /*     to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* mongoose-v can count cycles. :) */
#include <bspIo.h>

int Read_timer()
{
  rtems_unsigned32  clicks;
  rtems_unsigned32  total;
  rtems_unsigned32  tcr;

  clicks = MONGOOSEV_READ_REGISTER(
    TIMER_BASE,
    MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER
  );
  total = 0xffffffff - clicks;

  tcr = MONGOOSEV_READ_REGISTER( TIMER_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER );
  if ( tcr & MONGOOSEV_TIMER_CONTROL_TIMEOUT )
    printk( "MG5 timer overran\n" );

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in cycle units */

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
