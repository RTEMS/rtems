/**
 * @file benchmark_timer.c
 *
 * @ingroup tms570
 *
 * @brief clock functions definitions.
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <bsp/system-clocks.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead = false;

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = _CPU_Counter_read();
}

benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t delta = _CPU_Counter_read() - benchmark_timer_base;

  if (benchmark_timer_find_average_overhead) {
    return delta;
  } else {
    /* TODO check on hardware */
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
