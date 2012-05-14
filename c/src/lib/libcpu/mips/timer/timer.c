/*  timer.c
 *
 *  This file contains the initialization code for the IDT 4650 timer driver.
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  derived from src/lib/libbsp/no_cpu/no_bsp/timer/timer.c
 *
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().  benchmark_timer_read() usually returns
 *  the number of microseconds since benchmark_timer_initialize() exitted.
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
 */

/*
 *  Rather than deleting this, it is commented out to (hopefully) help
 *  the submitter send updates.
 *
 *  static char _sccsid[] = "@(#)timer.c 08/20/96     1.5\n";
 */


#include <rtems.h>

#define CLOCKS_PER_MICROSECOND ( CPU_CLOCK_RATE_MHZ )
#define TIMER_MAX_VALUE 0xffffffff

extern uint32_t   mips_read_timer( void );

static bool benchmark_timer_find_average_overhead;
static uint32_t   benchmark_timer_initial_value = 0;

void benchmark_timer_initialize( void )
{
   benchmark_timer_initial_value = mips_read_timer();
  /*
   *  Somehow start the timer
   */

  /* Timer on 4650 is always running */
}

/*
 *  The following controls the behavior of benchmark_timer_read().
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      8  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in cycles. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

int benchmark_timer_read( void )
{
  uint64_t   clicks;
  uint32_t   total;

  /*
   *  Read the timer and see how many clicks it has been since we started.
   */

  clicks = mips_read_timer();   /* XXX: read some HW here */
  if (clicks < benchmark_timer_initial_value)
  {
      clicks += TIMER_MAX_VALUE;
  }
  clicks -= benchmark_timer_initial_value;

  /*
   *  Total is calculated by taking into account the number of timer overflow
   *  interrupts since the timer was initialized and clicks since the last
   *  interrupts.
   */
#if 0 /* leave total in number of cycles */
   total = clicks / CLOCKS_PER_MICROSECOND;
#else
   total = clicks;
#endif

  if ( benchmark_timer_find_average_overhead == 1 )
    return total;          /* in # cycles units */
  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */
  /*
   *  leave total in cycles
   */
      return (total - AVG_OVERHEAD);
    }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
