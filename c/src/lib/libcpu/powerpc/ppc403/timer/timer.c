/*  timer.c
 *
 *  This file manages the interval timer on the PowerPC 405.
 *  We shall use the bottom 32 bits of the timebase register,
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1.1/timer/timer.c:
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 *  Further mods for PPC405EX/EXr by Michael Hamel
 *
 *  $Id$
 *
 */

#include <rtems.h>
#include <libcpu/powerpc-utility.h>

extern uint32_t bsp_timer_least_valid;
extern uint32_t bsp_timer_average_overhead;

static volatile uint32_t	startedAt;
static rtems_boolean		subtractOverhead;

void Timer_initialize()
{
  /* We are going to rely on clock.c to sort out where the clock comes from */
  startedAt = ppc_time_base();
}

int Read_timer()
{
	uint32_t   clicks, total;

	clicks = ppc_time_base();
	total = clicks - startedAt;
	if ( ! subtractOverhead )
		return total;          /* in XXX microsecond units */
	else if ( total < bsp_timer_least_valid )
		return 0;            /* below timer resolution */
	else
		return (total - bsp_timer_average_overhead);
}

rtems_status_code Empty_function( void )
{
	return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead( rtems_boolean find_flag)
{
	subtractOverhead = find_flag;
}
