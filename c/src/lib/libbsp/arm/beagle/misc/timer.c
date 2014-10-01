/**
 * @file
 *
 * @ingroup beagle
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <rtems/timerdrv.h>

#include <bsp.h>

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = beagleboard_timer();
}

uint32_t benchmark_timer_read(void)
{
  return beagleboard_timer() - benchmark_timer_base;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_average_overhead
)
{
  /* VOID */
}
