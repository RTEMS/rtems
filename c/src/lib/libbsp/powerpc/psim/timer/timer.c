/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer on
 *  the MEC.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <assert.h>

#include <bsp.h>

rtems_unsigned64 Timer_driver_Start_time;

rtems_boolean Timer_driver_Find_average_overhead;

static inline rtems_unsigned64 PPC_Get_timebase_register( void )
{
  rtems_unsigned32 tbr_low;
  rtems_unsigned32 tbr_high;
  rtems_unsigned32 tbr_high_old;
  rtems_unsigned64 tbr;

  do {
    asm volatile( "mftbu %0" : "=r" (tbr_high_old));
    asm volatile( "mftb  %0" : "=r" (tbr_low));
    asm volatile( "mftbu %0" : "=r" (tbr_high));
  } while ( tbr_high_old != tbr_high );

  tbr = tbr_high;
  tbr <<= 32;
  tbr |= tbr_low;
  return tbr;
}

void Timer_initialize()
{
  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */


  Timer_driver_Start_time = PPC_Get_timebase_register();
  

}

#define AVG_OVERHEAD     24  /* It typically takes 24 instructions */
                             /*     to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* psim can count instructions. :) */

int Read_timer()
{
  rtems_unsigned64  clicks;
  rtems_unsigned64  total64;
  rtems_unsigned32  total;

  /* approximately CLOCK_SPEED clicks per microsecond */

  clicks = PPC_Get_timebase_register();

  assert( clicks > Timer_driver_Start_time );

  total64 = clicks - Timer_driver_Start_time;

  assert( total64 <= 0xffffffff );  /* fits into a unsigned32 */

  total = (rtems_unsigned32) total64;

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
