/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

	.global _CPU_Context_volatile_clobber

_CPU_Context_volatile_clobber:

#ifdef PPC_MULTILIB_FPU
.macro CLOBBER_F i
	addi	r4, r3, 0x100 + \i
	stw	r4, 32(r1)
	addi	r4, r3, 0x200 + \i
	stw	r4, 32 + 4(r1)
	lfd	\i, 32(r1)
.endm

	PPC_REG_STORE_UPDATE	r1, -96(r1)

	/* Negate FPSCR[FPRF] bits */
	mffs	f0
	stfd	f0, 32(r1)
	lwz	r0, 36(r1)
	nor	r3, r0, r0
	rlwinm	r0, r0, 0, 20, 14
	rlwinm	r3, r3, 0, 15, 19
	or	r0, r3, r0
	stw	r0, 36(r1)
	lfd	f0, 32(r1)
	mtfsf	0xff, f0

	CLOBBER_F 0
	CLOBBER_F 1
	CLOBBER_F 2
	CLOBBER_F 3
	CLOBBER_F 4
	CLOBBER_F 5
	CLOBBER_F 6
	CLOBBER_F 7
	CLOBBER_F 8
	CLOBBER_F 9
	CLOBBER_F 10
	CLOBBER_F 11
	CLOBBER_F 12
	CLOBBER_F 13
	addi	r1, r1, 96
#endif

#ifdef PPC_MULTILIB_ALTIVEC
.macro CLOBBER_V i
	addi	r4, r3, 0x300 + \i
	stw	r4, 32(r1)
	addi	r4, r3, 0x400 + \i
	stw	r4, 32 + 4(r1)
	addi	r4, r3, 0x500 + \i
	stw	r4, 32 + 8(r1)
	addi	r4, r3, 0x600 + \i
	stw	r4, 32 + 12(r1)
	li	r4, 32
	lvx	\i, r1, r4
.endm

	PPC_REG_STORE_UPDATE	r1, -96(r1)

	/* Negate VSCR[SAT] bit */
	mfvscr	v0
	li	r3, 44
	stvewx	v0, r1, r3
	lwz	r0, 44(r1)
	nor	r3, r0, r0
	rlwinm	r0, r0, 0, 0, 30
	rlwinm	r3, r3, 0, 31, 31
	or	r0, r3, r0
	stw	r0, 44(r1)
	li	r3, 44
	lvewx	v0, r1, r3
	mtvscr	v0

	CLOBBER_V 0
	CLOBBER_V 1
	CLOBBER_V 2
	CLOBBER_V 3
	CLOBBER_V 4
	CLOBBER_V 5
	CLOBBER_V 6
	CLOBBER_V 7
	CLOBBER_V 8
	CLOBBER_V 9
	CLOBBER_V 10
	CLOBBER_V 11
	CLOBBER_V 12
	CLOBBER_V 13
	CLOBBER_V 14
	CLOBBER_V 15
	CLOBBER_V 16
	CLOBBER_V 17
	CLOBBER_V 18
	CLOBBER_V 19
	addi	r1, r1, 96
#endif

	addi	r4, r3, 10
	rlwinm	r4, r4, 0, 20, 7
	mfcr	r5
	rlwinm	r5, r5, 0, 8, 19
	or	r4, r4, r5
	mtcr	r4
	addi	r4, r3, 11
	mtctr	r4
	addi	r4, r3, 12
	mtxer	r4
	addi	r0, r3, 13
	addi	r4, r3, 1
	addi	r5, r3, 2
	addi	r6, r3, 3
	addi	r7, r3, 4
	addi	r8, r3, 5
	addi	r9, r3, 6
	addi	r10, r3, 7
	addi	r11, r3, 8
	addi	r12, r3, 9
	blr
