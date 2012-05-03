/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer.
 *
 *  Notes:
 *
 *  BSP_TIMER_AVG_OVERHEAD and BSP_TIMER_LEAST_VALID are required to be
 *  provided in bsp.h
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <bsp.h>
#include <rtems/btimer.h>

uint64_t         Timer_driver_Start_time;

bool benchmark_timer_find_average_overhead;

/*
 * benchmark_timer_initialize
 */

void benchmark_timer_initialize()
{

  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */

  Timer_driver_Start_time = PPC_Get_timebase_register();
}

/*
 *  benchmark_timer_read
 */

uint32_t benchmark_timer_read()
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

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in "clicks" of the decrementer units */

  if ( total < BSP_TIMER_LEAST_VALID )
    return 0;            /* below timer resolution */

  return BSP_Convert_decrementer(total - BSP_TIMER_AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
