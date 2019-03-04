/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQMMU
 *
 * @brief qoriq_tlb1_write() and qoriq_tlb1_invalidate() implementation.
 */

/*
 * Copyright (c) 2011, 2017 embedded brains GmbH.  All rights reserved.
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

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

	.global qoriq_tlb1_write
	.global qoriq_tlb1_invalidate
	.global qoriq_tlb1_invalidate_all_by_ts

	.section ".bsp_start_text", "ax"

qoriq_tlb1_write:
	rlwinm	r3, r3, 16, 10, 15
#ifdef __powerpc64__
	rldicr	r8, r8, 0, 51
#else
	rlwinm	r8, r8, 0, 0, 19
#endif
	oris	r3, r3, 0x1000
	mtspr	FSL_EIS_MAS0, r3
	oris	r4, r4, 0xc000
	rlwinm	r9, r9, 8, 20, 23
	or	r9, r4, r9
	mtspr	FSL_EIS_MAS1, r9
	or	r5, r8, r5
	mtspr	FSL_EIS_MAS2, r5
	or	r6, r8, r6
	mtspr	FSL_EIS_MAS3, r6
#ifdef __powerpc64__
	srdi	r8, r8, 32
	or	r7, r7, r8
	mtspr	FSL_EIS_MAS7, r7
#endif
	mtspr	FSL_EIS_MAS7, r7
#if defined(QORIQ_HAS_HYPERVISOR_MODE) && !defined(QORIQ_IS_HYPERVISOR_GUEST)
	li	r0, 0
	mtspr	FSL_EIS_MAS8, r0
#endif
	isync
	msync
	tlbwe
	isync
	blr

qoriq_tlb1_invalidate:
	rlwinm	r3, r3, 16, 10, 15
	oris	r3, r3, 0x1000
	mtspr	FSL_EIS_MAS0, r3
	li	r0, 0
	mtspr	FSL_EIS_MAS1, r0
	mtspr	FSL_EIS_MAS2, r0
	mtspr	FSL_EIS_MAS3, r0
	mtspr	FSL_EIS_MAS7, r0
#if defined(QORIQ_HAS_HYPERVISOR_MODE) && !defined(QORIQ_IS_HYPERVISOR_GUEST)
	mtspr	FSL_EIS_MAS8, r0
#endif
	isync
	msync
	tlbwe
	isync
	blr

/* r3 = 0 for TS0, 1 for TS1 */
qoriq_tlb1_invalidate_all_by_ts:
	mflr	r12
	li	r11, QORIQ_TLB1_ENTRY_COUNT
	mtctr	r11
	li	r11, 0
	mr	r10, r3

2:
	rlwinm	r0, r11, 16, 10, 15
	oris	r0, r0, (FSL_EIS_MAS0_TLBSEL >> 16)
	mtspr	FSL_EIS_MAS0, r0
	tlbre
	mfspr	r0, FSL_EIS_MAS1
	rlwinm	r0, r0, 20, 31, 31
	cmpw	r0, r10
	bne	1f
	mr	r3, r11
	bl	qoriq_tlb1_invalidate
1:
	addi	r11, r11, 1
	bdnz	2b
	mtlr	r12
	blr
