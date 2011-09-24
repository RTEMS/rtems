/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/btimer.h>

static bool benchmark_timer_find_average_overhead = false;

void benchmark_timer_initialize(void)
{
  /* TODO */
}

uint32_t benchmark_timer_read(void)
{
  /* TODO */
  return 0;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_average_overhead
)
{
  benchmark_timer_find_average_overhead = find_average_overhead;
}
