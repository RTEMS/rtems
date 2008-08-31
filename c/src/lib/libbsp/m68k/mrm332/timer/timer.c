/*  Timer_init()
 *
 *  This routine is not implemented for this BSP.
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

rtems_boolean benchmark_timer_find_average_overhead;

extern rtems_isr Clock_isr(void);

void benchmark_timer_initialize( void )
{
}

/*
 *  The following controls the behavior of benchmark_timer_read().
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
int benchmark_timer_read( void )
{
  uint32_t         total;
  total = 0;

  if ( benchmark_timer_find_average_overhead == 1 )
    return total;          /* in XXX microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return (total - AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(
  rtems_boolean find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
