/*  timer.c
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
 *
 *  This version returns the number of nanoseconds since Timer_initialize() 
 *  exited, since the '604 is very fast at 300MHz.  Actual resolution is
 *  60 nsec.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

rtems_unsigned32 Timer_start;
rtems_boolean Timer_driver_Find_average_overhead;

void Timer_initialize( void )
{
    rtems_unsigned32 clicks;

    asm volatile ("mfspr %0,268" : "=r" (clicks) :);
    Timer_start = clicks;
}

/*
 *  The following controls the behavior of Read_timer().
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      7  /* On average, it takes 7.4 nanoseconds */
                             /* (Y countdowns) to start/stop the timer. */

#define NSEC_PER_TICK    60  

int Read_timer( void )
{
    rtems_unsigned32 clicks;
    rtems_unsigned32 total;

    /*
     *  Read the timer and see how many clicks it has been since we started.
     *  Timer overflows every 257 seconds, so multiple overflows are deemed
     *  impossible.
     */

    asm volatile ("mfspr %0,268" : "=r" (clicks) :);
    total = (clicks - Timer_start);

    if ( Timer_driver_Find_average_overhead == 1 ) {
        return total * NSEC_PER_TICK;          /* in nanoseconds */
    } else {
        /*
        *   convert total into nanoseconds
        */
        return total * NSEC_PER_TICK - AVG_OVERHEAD;
    }
}

/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

rtems_status_code Empty_function( void ) {
    return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(rtems_boolean find_flag) {
    Timer_driver_Find_average_overhead = find_flag;
}

