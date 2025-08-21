/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * RTEMS TQM8xx BSP
 *
 * This file contains the console driver.
 */

/*
 * Copyright (C) 2008 Thomas Doerfler, embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * benchmark_timer_initialize()
 *
 * Use TIMER 1 and TIMER 2 for Timing Test Suite
 *
 * this is derived from "timer.c" available in the m68k/gen68360 BSP
 * adapted by Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 */

/*
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>
#include <mpc8xx.h>

bool benchmark_timer_find_average_overhead;

void
benchmark_timer_initialize (void)
{
	/*
	 * Reset timers 1 and 2
	 */
	m8xx.tgcr &= ~0x00FF;
	m8xx.tcn1 = 0;
	m8xx.tcn2 = 0;
	m8xx.ter1 = 0xFFFF;
	m8xx.ter2 = 0xFFFF;

	/*
	 * Cascade timers 1 and 2
	 */
	m8xx.tgcr |= M8xx_TGCR_CAS2;

	/*
	 * Configure timers 1 and 2 to a single 32-bit, BUS_clock timer.
	 */
	m8xx.tmr2 = (0 << 8) | 0x2;
	m8xx.tmr1 = 0;

	/*
	 * Start the timers
	 */
	m8xx.tgcr |=  0x0011;
}

/*
 * Return timer value in microsecond units
 */
benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t retval;
  retval = *(uint32_t*)&m8xx.tcn1;
  retval = retval * 1000000LL / BSP_bus_frequency;
  return retval;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
