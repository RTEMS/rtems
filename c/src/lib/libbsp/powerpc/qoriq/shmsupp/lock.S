/**
 * @file
 *
 * @ingroup QorIQInterCom
 *
 * @brief qoriq_spin_lock() and qoriq_spin_unlock() implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/powerpc-utility.h>

	.global qoriq_spin_lock
	.global qoriq_spin_unlock

qoriq_spin_lock:
	li	r0, 1
	mfmsr	r4
	GET_INTERRUPT_MASK r5
	andc	r5, r4, r5
	b	2f
1:
	mtmsr	r4
2:
	lwarx	r6, r0, r3
	cmpwi	r6, 0
	bne	2b
	mtmsr	r5
	stwcx.	r0, r0, r3
	bne	1b
	isync
	mr	r3, r4
	blr

qoriq_spin_unlock:
	msync
	li	r0, 0
	stw	r0, 0(r3)
	mtmsr	r4
	blr
