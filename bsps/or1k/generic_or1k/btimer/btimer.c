/**
 * @file
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2014-2015 by Hesham ALMatary
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp/generic_or1k.h>
#include <rtems/score/or1k-utility.h>

#define OR1K_NANOSECONDS_PER_CLK_CYCLE 10

static bool benchmark_timer_find_average_overhead = false;
static uint64_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);
}

#define AVG_OVERHEAD  0
#define LEAST_VALID   1

benchmark_timer_t benchmark_timer_read( void )
{
  uint64_t         clicks;
  uint64_t         total;
  uint64_t         delta;
  /*
   *  Read the timer and see how many clicks (clock cycles)
   *  has passed since timer initialization.
   */
  clicks = _OR1K_mfspr(CPU_OR1K_SPR_TTCR);

  delta = clicks - benchmark_timer_base;

  /* total in nanoseconds */
  total = OR1K_NANOSECONDS_PER_CLK_CYCLE * (delta);

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in nanoseconds microsecond units */
  else {
    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */

      return (total - AVG_OVERHEAD);
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
