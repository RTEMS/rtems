/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer.
 *
 *  Notes: NONE
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <assert.h>
#include <rtems.h>
#include <bsp.h>

rtems_unsigned64 Timer_driver_Start_time;

rtems_boolean Timer_driver_Find_average_overhead = 0;
unsigned clicks_overhead = 0;

/*
 * Timer Get overhead
 */

int Timer_get_clicks_overhead()
{
  rtems_unsigned64  clicks;

  PPC_Set_timebase_register((unsigned64) 0);
  clicks = PPC_Get_timebase_register();
  assert(clicks <= 0xffffffff);
  clicks_overhead = (unsigned) clicks;
  return clicks_overhead;
}

/*
 * Timer_initialize 
 */
void Timer_initialize()
{

  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */

  if (clicks_overhead == 0) clicks_overhead = Timer_get_clicks_overhead();
  PPC_Set_timebase_register((unsigned64) 0);
}


/*
 *  Read_timer
 */

int Read_timer()
{
  rtems_unsigned64  total64;
  rtems_unsigned32  total;

  /* approximately CLOCK_SPEED clicks per microsecond */

  total64 = PPC_Get_timebase_register();

  assert( total64 <= 0xffffffff );  /* fits into a unsigned32 */

  total = (rtems_unsigned32) total64;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in "clicks" of the decrementer units */

  return (int) BSP_Convert_decrementer(total - clicks_overhead);
}

unsigned long long Read_long_timer()
{
  rtems_unsigned64  total64;

  total64 = PPC_Get_timebase_register();
  return BSP_Convert_decrementer(total64 - clicks_overhead);
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
