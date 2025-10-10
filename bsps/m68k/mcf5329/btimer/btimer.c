/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  Timer Init
 *
 *  Use the last DMA timer (DTIM3) as the diagnostic timer.
 */

/*
 * Copyright (c) 2005 Eric Norum <eric@norum.ca>
 *
 * COPYRIGHT (c) 2005.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>

void benchmark_timer_initialize(void)
{
  uint32_t preScaleDivisor = bsp_get_BUS_clock_speed() / 1000000;

  MCF_DTIM3_DTMR = 0;
  MCF_DTIM3_DTMR = MCF_DTIM_DTMR_PS(preScaleDivisor - 1) |
    MCF_DTIM_DTMR_CLK_DIV1 | MCF_DTIM_DTMR_RST;
}

/*
 * Return timer value in microsecond units
 */
benchmark_timer_t benchmark_timer_read(void)
{
  return MCF_DTIM3_DTCN;
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
  (void) find_flag;

}
