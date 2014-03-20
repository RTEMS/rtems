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

	.set	noat
	.section	.text

	.global _CPU_Context_volatile_clobber

_CPU_Context_volatile_clobber:

	subi	at, r4, 11
	subi	r2, r4, 12
	subi	r3, r4, 13
	subi	r5, r4, 1
	subi	r6, r4, 2
	subi	r7, r4, 3
	subi	r8, r4, 4
	subi	r9, r4, 5
	subi	r10, r4, 6
	subi	r11, r4, 7
	subi	r12, r4, 8
	subi	r13, r4, 9
	subi	r14, r4, 10
	subi	r15, r4, 11
	ret
