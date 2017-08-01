/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#if defined(__powerpc64__)
	.section ".init","ax"
	addi	1,1,96
	ld	0,16(1)
	mtlr	0
	blr

	.section ".fini","ax"
	addi	1,1,96
	ld	0,16(1)
	mtlr	0
	blr
#endif
