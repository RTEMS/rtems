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

	.global qoriq_l1cache_invalidate

	.section ".bsp_start_text", "ax"

qoriq_l1cache_invalidate:

	/* Invalidate L1 data cache */
	mfspr	r3, FSL_EIS_L1CSR0
	ori	r3, r3, FSL_EIS_L1CSR0_CFI
	mtspr	FSL_EIS_L1CSR0, r3
1:
	mfspr	r3, FSL_EIS_L1CSR0
	andi.	r3, r3, FSL_EIS_L1CSR0_CFI
	bne	1b
	isync

	/* Invalidate L1 instruction cache */
	mfspr	r3, FSL_EIS_L1CSR1
	ori	r3, r3, FSL_EIS_L1CSR1_ICFI
	mtspr	FSL_EIS_L1CSR1, r3
1:
	mfspr	r3, FSL_EIS_L1CSR1
	andi.	r3, r3, FSL_EIS_L1CSR1_ICFI
	bne	1b
	isync

	blr
