/*  Timer_init()
 *
 *  This routine initializes a timer in efi68k's DP8570A TCP
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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


#include <bsp.h>

rtems_boolean Timer_driver_Find_average_overhead;

extern rtems_isr Clock_isr();

void Timer_initialize( void )
{
}

/*
 *  The following controls the behavior of Read_timer().
 *
 *  FIND_AVG_OVERHEAD *  instructs the routine to return the "raw" count.
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/*
 * Return timer value in 1/2-microsecond units
 */
int Read_timer( void )
{
  rtems_unsigned32 total;
  total = 0;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total - AVG_OVERHEAD);
}


/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

rtems_status_code Empty_function(void)
{
    return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
