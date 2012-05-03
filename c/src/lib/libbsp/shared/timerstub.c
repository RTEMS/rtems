/*
 *  This file implements a stub benchmark timer that is sufficient to
 *  satisfy linking the RTEMS Benchmarks.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 */

#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;

void benchmark_timer_initialize(void)
{
}

uint32_t benchmark_timer_read(void)
{
  if (benchmark_timer_find_average_overhead)
    return 1;
  return 0;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
