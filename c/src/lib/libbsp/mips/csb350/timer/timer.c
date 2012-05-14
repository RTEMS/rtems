/*
 *  This file implements a benchmark timer using the count/compare
 *  CP0 registers.
 *
 *  Copyright (c) 2005 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;
uint32_t tstart;

void benchmark_timer_initialize(void)
{
    __asm__ volatile ("mfc0 %0, $9\n" : "=r" (tstart));
    /* tick time in picooseconds */
}

#define AVG_OVERHEAD      0  /* It typically takes N instructions */
                             /*     to start/stop the timer. */
#define LEAST_VALID       1  /* Don't trust a value lower than this */
                             /* tx39 simulator can count instructions. :) */

uint32_t benchmark_timer_read(void)
{
  uint32_t  total;
  uint32_t  cnt;

  __asm__ volatile ("mfc0 %0, $9\n" : "=r" (cnt));

  total = cnt - tstart;
  total = (total * 1000) / 396; /* convert to nanoseconds */


  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
