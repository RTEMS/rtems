/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

	.global _CPU_Context_volatile_clobber

_CPU_Context_volatile_clobber:

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
