/*  timer.c
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
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
#include <bsp.h>
#include <c4xio.h>

rtems_unsigned32 Timer_interrupts;
rtems_boolean Timer_driver_Find_average_overhead;

static unsigned32 start;

void Timer_initialize( void )
{

  /*
   *  Timer has never overflowed.  This may not be necessary on some
   *  implemenations of timer but ....
   */

  
  c4x_timer_stop(C4X_TIMER_0);
  c4x_timer_set_period(C4X_TIMER_0, 0xffffffff); /* so no interupts */
  c4x_timer_start(C4X_TIMER_0);
  start = c4x_timer_get_counter(C4X_TIMER_0);

  Timer_interrupts = 0;

  /*
   *  Somehow start the timer
   */
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

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

int Read_timer( void )
{
  rtems_unsigned32 clicks;
  rtems_unsigned32 total;
  int            tmp;

  /*
   *  Read the timer and see how many clicks it has been since we started.
   */

  clicks = c4x_timer_get_counter(C4X_TIMER_0);
  clicks -= start;

  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */

  total = clicks * 1;

  if ( Timer_driver_Find_average_overhead == 1 ) {
    return total;          /* in count units where each count is */
                           /* 1 / (clock frequency/2) */
  } else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  Somehow convert total into microseconds
   */
      tmp = (int) ((float) total * ((1.0 / 25.0)));
      return (tmp - AVG_OVERHEAD);
    }
}

/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */

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

