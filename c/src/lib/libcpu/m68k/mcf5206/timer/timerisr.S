/*
 *  Handle MCF5206 TIMER2 interrupts.
 *
 * All code in this routine is pure overhead which can perturb the
 * accuracy of RTEMS' timing test suite.
 *
 * See also:	benchmark_timer_read()
 *
 * To reduce overhead this is best to be the "rawest" hardware interupt
 * handler you can write.  This should be the only interrupt which can
 * occur during the measured time period.
 *
 * An external counter, Timer_interrupts, is incremented.
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  This file based on work:
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include "bsp.h"
#include "mcf5206/mcf5206e.h"

BEGIN_CODE
	PUBLIC(timerisr)
SYM(timerisr):
	move.l   a0, a7@-
	move.l   #MCF5206E_TER(BSP_MEM_ADDR_IMM, 2), a0
	move.b   # (MCF5206E_TER_REF + MCF5206E_TER_CAP), (a0)
	addq.l	 #1,SYM(Timer_interrupts) | increment timer value
	move.l   a7@+, a0
	rte
END_CODE
END
