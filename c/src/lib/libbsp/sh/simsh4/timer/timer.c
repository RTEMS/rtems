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

bool benchmark_timerfind_average_overhead;

/* External assembler interrupt handler routine */
extern rtems_isr timerisr(rtems_vector_number vector);

/* benchmark_timerinitialize --
 *     Initialize timer 2 for accurate time measurement.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
benchmark_timerinitialize(void)
{
    return;
}

/*
 *  The following controls the behavior of benchmark_timerread().
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

/* benchmark_timerread --
 *     Read timer value in microsecond units since timer start.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     number of microseconds since timer has been started
 */
int
benchmark_timerread( void )
{
    return 0;
}

/* benchmark_timerempty_function --
 *     Empty function call used in loops to measure basic cost of looping
 *     in Timing Test Suite.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
benchmark_timerempty_function(void)
{
    return RTEMS_SUCCESSFUL;
}

/* benchmark_timerdisable_subtracting_average_overhead --
 *     This routine is invoked by the "Check Timer" (tmck) test in the
 *     RTEMS Timing Test Suite. It makes the benchmark_timerread routine not
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
benchmark_timerdisable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timerfind_average_overhead = find_flag;
}
