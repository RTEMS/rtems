/*
 *  Timer Init
 *
 *  This module implements benchmark stub for simulator.
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

uint32_t         Timer_interrupts;

bool benchmark_timer_find_average_overhead;

/* External assembler interrupt handler routine */
extern rtems_isr timerisr(rtems_vector_number vector);

/* benchmark_timer_initialize --
 *     Initialize timer 2 for accurate time measurement.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
benchmark_timer_initialize(void)
{
    return;
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

#define AVG_OVERHEAD      0  /* It typically takes 2.0 microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

/* benchmark_timer_read --
 *     Read timer value in microsecond units since timer start.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     number of microseconds since timer has been started
 */
int
benchmark_timer_read( void )
{
    return 0;
}

/* benchmark_timer_disable_subtracting_average_overhead --
 *     This routine is invoked by the "Check Timer" (tmck) test in the
 *     RTEMS Timing Test Suite. It makes the benchmark_timer_read routine not
 *     subtract the overhead required to initialize and read the benchmark
 *     timer.
 *
 * PARAMETERS:
 *     find_flag - boolean flag, TRUE if overhead must not be subtracted.
 *
 * RETURNS:
 *     none
 */
void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
