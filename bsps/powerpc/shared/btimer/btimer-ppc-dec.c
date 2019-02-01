/**
 *  @file
 *  @brief
 *
 *  This file implements a benchmark timer using the PPC Timebase Register.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems.h>
#include <rtems/btimer.h>
#include <assert.h>
#include <libcpu/powerpc-utility.h>

#ifndef BSP_Convert_decrementer
#define BSP_Convert_decrementer(value) (value)
#endif

uint64_t   Timer_driver_Start_time;

bool benchmark_timer_find_average_overhead = false;
unsigned clicks_overhead = 0;

/*
 * Timer Get overhead
 */
static int Timer_get_clicks_overhead(void)
{
  uint64_t    clicks;

  PPC_Set_timebase_register((uint64_t) 0);
  clicks = PPC_Get_timebase_register();
  assert(clicks <= 0xffffffff);
  clicks_overhead = (unsigned) clicks;
  return clicks_overhead;
}

/*
 * benchmark_timer_initialize
 */
void benchmark_timer_initialize(void)
{

  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */

  if (clicks_overhead == 0) clicks_overhead = Timer_get_clicks_overhead();
  PPC_Set_timebase_register((uint64_t) 0);
}


/*
 *  benchmark_timer_read
 */

benchmark_timer_t benchmark_timer_read(void)
{
  uint64_t    total64;
  uint32_t    total;

  /* approximately CLOCK_SPEED clicks per microsecond */

  total64 = PPC_Get_timebase_register();

  assert( total64 <= 0xffffffff );  /* fits into a uint32_t   */

  total = (uint32_t) total64;

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in "clicks" of the decrementer units */

  return (int) BSP_Convert_decrementer(total - clicks_overhead);
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
