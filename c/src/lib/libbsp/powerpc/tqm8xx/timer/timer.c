/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
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
 *  http://www.rtems.com/license/LICENSE.
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
uint32_t benchmark_timer_read(void)
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
