/*  gettime.s
 *
 *  This file contains the assembly code for the IDT 4650 timer driver.
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 */

/* @(#)gettime.S       08/20/96     1.2 */

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/asm.h>

FRAME(mips_read_timer,sp,0,ra)
	.set noreorder
        mfc0 v0,C0_COUNT
        nop
        j ra
	.set reorder
ENDFRAME(mips_read_timer)
