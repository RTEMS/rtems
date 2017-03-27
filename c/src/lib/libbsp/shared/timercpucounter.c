/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/btimer.h>
#include <rtems/counter.h>

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = rtems_counter_read();
}

benchmark_timer_t benchmark_timer_read(void)
{
  return rtems_counter_difference(rtems_counter_read(), benchmark_timer_base);
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_average_overhead
)
{
  (void) find_average_overhead;
}
