/*  timer.c
 *
 *  This file implements a benchmark timer using the PPC Timebase
 *
 *  Notes: NONE
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <assert.h>
#include <rtems.h>
#include <bsp.h>
#include <libcpu/powerpc-utility.h>


uint64_t   Timer_driver_Start_time;

bool benchmark_timer_find_average_overhead = false;
unsigned clicks_overhead = 0;

/*
 * Timer Get overhead
 */

int Timer_get_clicks_overhead(void)
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

int benchmark_timer_read(void)
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

unsigned long long Read_long_timer(void)
{
  uint64_t    total64;

  total64 = PPC_Get_timebase_register();
  return BSP_Convert_decrementer(total64 - clicks_overhead);
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
