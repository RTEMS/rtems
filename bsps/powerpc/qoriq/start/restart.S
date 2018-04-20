/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#define FIRST_TLB 0

#define SCRATCH_TLB QORIQ_TLB1_ENTRY_COUNT - 1

	.global qoriq_restart_secondary_processor

	.section ".bsp_start_text", "ax"

qoriq_restart_secondary_processor:

	mr	r14, r3

	/* Invalidate all TS1 MMU entries */
	li	r3, 1
	bl	qoriq_tlb1_invalidate_all_by_ts

	/* Add TS1 entry for the first 4GiB of RAM */
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_I
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write

	bl	qoriq_l1cache_invalidate

	/* Set MSR and use TS1 for address translation */
	LWI	r0, QORIQ_INITIAL_MSR | MSR_IS | MSR_DS
	mtmsr	r0
	isync

	/* Invalidate all TS0 MMU entries */
	li	r3, 0
	bl	qoriq_tlb1_invalidate_all_by_ts

	/* Add TS0 entry for the first 4GiB of RAM */
	li	r3, FIRST_TLB
	li	r4, 0
	li	r5, FSL_EIS_MAS2_I
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write

	/* Use TS0 for address translation */
	LWI	r0, QORIQ_INITIAL_MSR
	mtmsr	r0
	isync

	bl	qoriq_l1cache_invalidate

	/* Wait for restart request */
	li	r0, 0
.Lrestartagain:
	lwz	r4, 4(r14)
	cmpw	r0, r4
	beq	.Lrestartagain
	isync
	mtctr	r4
	lwz	r3, 12(r14)
	bctr
