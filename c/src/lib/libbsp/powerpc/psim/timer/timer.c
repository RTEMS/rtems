/*
 *  This file implements a benchmark timer using the PPC decrement register.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *
 *  $Id$
 */

#include <assert.h>

#include <bsp.h>

uint64_t         Timer_driver_Start_time;

rtems_boolean Timer_driver_Find_average_overhead;

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
  uint64_t          clicks;
  uint64_t          total64;
  uint32_t          total;

  /* approximately CLOCK_SPEED clicks per microsecond */

  clicks = PPC_Get_timebase_register();

  assert( clicks > Timer_driver_Start_time );

  total64 = clicks - Timer_driver_Start_time;

  assert( total64 <= 0xffffffff );  /* fits into a uint32_t   */

  total = (uint32_t) total64;

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
