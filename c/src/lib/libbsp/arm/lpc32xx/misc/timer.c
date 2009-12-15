/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2008, 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/timerdrv.h>

static bool benchmark_timer_find_average_overhead = false;

static uint32_t benchmark_timer_base;

/* TODO */
static uint32_t lpc32xx_timer(void)
{
  return 0;
}

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = lpc32xx_timer();
}

uint32_t benchmark_timer_read(void)
{
  uint32_t delta = lpc32xx_timer() - benchmark_timer_base;

  if (benchmark_timer_find_average_overhead) {
    return delta;
  } else {
    /* TODO */
    return 0;
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_average_overhead)
{
  benchmark_timer_find_average_overhead = find_average_overhead;
}
