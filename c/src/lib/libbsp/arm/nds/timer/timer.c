/*
 * RTEMS for Nintendo DS performance timer driver.
 *
 * Copyright (c) 2008 by Benjamin Ratier <agho.pwn@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>
#include <nds.h>

bool benchmark_timer_find_average_overhead;

void
benchmark_timer_initialize (void)
{
  TIMER_CR (1) = 0x0000;
  TIMER_CR (2) = 0x0000;
  TIMER_DATA (1) = TIMER_FREQ (1000000);
  TIMER_DATA (2) = 0x0000;
  TIMER_CR (1) = TIMER_ENABLE | TIMER_DIV_1;
  TIMER_CR (2) = TIMER_ENABLE | TIMER_CASCADE;
}

/*
 * values taken out from gba bsp.
 */

#define AVG_OVERHEAD    3
#define LEAST_VALID     1

uint32_t
benchmark_timer_read (void)
{
  uint32_t ticks;

  /* stop counters */
  TIMER_CR (1) = 0x0000;
  TIMER_CR (2) = 0x0000;

  /* read counter */
  ticks = (TIMER_DATA (2) << 16) | TIMER_DATA (1);
  return ticks;
}

void
benchmark_timer_disable_subtracting_average_overhead (bool find_flag)
{
  benchmark_timer_find_average_overhead = find_flag;
}
