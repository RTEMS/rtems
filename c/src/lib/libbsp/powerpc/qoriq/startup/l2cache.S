/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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

#include <libcpu/powerpc-utility.h>

	.global qoriq_l2cache_flush_invalidate

	.section ".bsp_start_text", "ax"

qoriq_l2cache_flush_invalidate:

	/* Flush L2 cache */
	lwz	r4, 0(r3)
	oris	r4, r4, FSL_EIS_L2CSR0_L2FL >> 16
	stw	r4, 0(r3)
1:
	lwz	r4, 0(r3)
	andis.	r4, r4, FSL_EIS_L2CSR0_L2FL >> 16
	bne	1b
	isync

	/* Invalidate L2 cache */
	lwz	r4, 0(r3)
	oris	r4, r4, FSL_EIS_L2CSR0_L2FI >> 16
	stw	r4, 0(r3)
1:
	lwz	r4, 0(r3)
	andis.	r4, r4, FSL_EIS_L2CSR0_L2FI >> 16
	bne	1b
	isync

	blr
