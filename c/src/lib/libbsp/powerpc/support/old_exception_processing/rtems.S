/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the PowerPC implementation of RTEMS.
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
 *  Derived from c/src/exec/cpu/no_cpu/rtems.c:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <asm.h>

	BEGIN_CODE
/*
 *  RTEMS
 *
 *  This routine jumps to the directive indicated in r11.
 *  This routine is used when RTEMS is linked by itself and placed
 *  in ROM.  This routine is the first address in the ROM space for
 *  RTEMS.  The user "calls" this address with the directive arguments
 *  in the normal place.
 *  This routine then jumps indirectly to the correct directive
 *  preserving the arguments.  The directive should not realize
 *  it has been "wrapped" in this way.  The table "_Entry_points"
 *  is used to look up the directive.
 */

	ALIGN (4, 2)
	PUBLIC_PROC (RTEMS)
PROC (RTEMS):
#if (PPC_ABI == PPC_ABI_POWEROPEN)
	mflr	r0
	stw	r0, 8(r1)
	stwu	r1, -64(r1)

	/* Establish addressing */
	bl	base
base:
	mflr	r12
	addi	r12, r12, tabaddr - base

	lwz	r12, Entry_points-abase(r12)
	slwi	r11, r11, 2
	lwzx	r12, r12, r11

	stw	r2, 56(r1)
	lwz	r0, 0(r12)
	mtlr	r0
	lwz	r2, 4(r12)
	lwz	r11, 8(r12)
	blrl
	lwz	r2, 56(r1)
	addi	r1, r1, 64
	lwz	r0, 8(r1)
	mtlr	r0
#else
	mflr	r0
	stw	r0, 4(r1)
	stwu	r1, -16(r1)

	/* Establish addressing */
	bl	base
base:
	mflr	r12
	addi	r12, r12, tabaddr - base

	lwz	r12, Entry_points-abase(r12)
	slwi	r11, r11, 2
	lwzx	r11, r12, r11

	stw	r2, 8(r1)
#if (PPC_ABI != PPC_ABI_GCC27)
	stw	r13, 12(r1)
#endif
	mtlr	r11
	lwz	r11, irqinfo-abase(r12)
	lwz	r2, 0(r11)
#if (PPC_ABI != PPC_ABI_GCC27)
	lwz	r13, 4(r11)
#endif
	blrl
	lwz	r2, 8(r1)
#if (PPC_ABI != PPC_ABI_GCC27)
	lwz	r13, 12(r1)
#endif
	addi	r1, r1, 16
	lwz	r0, 4(r1)
	mtlr	r0
#endif
	blr


	/* Addressability stuff */
tabaddr:
abase:
	EXTERN_VAR (_Entry_points)
Entry_points:
	EXT_SYM_REF (_Entry_points)
#if (PPC_ABI != PPC_ABI_POWEROPEN)
	EXTERN_VAR (_CPU_IRQ_info)
irqinfo:
	EXT_SYM_REF (_CPU_IRQ_info)
#endif

#if (PPC_ABI == PPC_ABI_POWEROPEN)
	DESCRIPTOR (RTEMS)
#endif


