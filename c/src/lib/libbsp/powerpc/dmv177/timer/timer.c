/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer on
 *  the MEC.
 *
 *  The license and distribution terms for this file are in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <assert.h>

#include <bsp.h>

rtems_unsigned64 Timer_driver_Start_time;

rtems_boolean Timer_driver_Find_average_overhead;

/*PAGE
 *
 *  Timer_initialize
 *  
 *  This routine initializes the timer.  
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

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

/*  PAGE
 *
 *  Read_timer
 *
 *  This routine reads the timer.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      timer in ms units
 *
 */

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

/*  PAGE
 *
 *  Empty_function
 *
 *  This routine is called during the idle loop.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  
 *    status code of successful
 *
 *  Return values:      NONE
 *
 */

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

/*  PAGE
 *
 *  Set_find_average_overhead
 *
 *  This routine sets a global boolean to the value passed in.
 *
 *  Input parameters: 
 *    find_flag  - flag to indicate to find the average overhead.
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 *
 */

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
