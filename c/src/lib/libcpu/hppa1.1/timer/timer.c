/*  timer.c
 *
 *  This file manages the interval timer on the PA-RISC.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>

volatile rtems_unsigned32 Timer_starting;
rtems_boolean Timer_driver_Find_average_overhead;

void Timer_initialize()
{
  Timer_starting = get_itimer();
  set_eiem(0x80000000);
}

#define AVG_OVERHEAD      0  /* It typically takes 3.0 microseconds */
                             /* (6 countdowns) to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */

int Read_timer()
{
  rtems_unsigned32 clicks;
  rtems_unsigned32 total;

  clicks = get_itimer();

  total = clicks - Timer_starting;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
    return (total - AVG_OVERHEAD);
  }
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
