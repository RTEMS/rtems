/**
 * @file
 *
 * @ingroup lpc24xx
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
#include <bsp.h>
#include <rtems/btimer.h>
#include <rtems/timerdrv.h>

#include <bsp/system-clocks.h>

bool benchmark_timer_find_average_overhead = false;

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = lpc24xx_timer();
}

uint32_t benchmark_timer_read(void)
{
  uint32_t delta = lpc24xx_timer() - benchmark_timer_base;

  if (benchmark_timer_find_average_overhead) {
    return delta;
  } else {
    /* Value determined by tmck for NCS board */
    if (delta > 74) {
      return delta - 74;
    } else {
      return 0;
    }
  }
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_average_overhead )
{
  benchmark_timer_find_average_overhead = find_average_overhead;
}
