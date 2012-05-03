/**
 * @file
 *
 * @ingroup QorIQMMU
 *
 * @brief qoriq_tlb1_write() and qoriq_tlb1_invalidate() implementation.
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

	.global qoriq_tlb1_write
	.global qoriq_tlb1_invalidate
	.global qoriq_tlb1_ts_0_only

	.section ".bsp_start_text", "ax"

qoriq_tlb1_write:
	rlwinm	r3, r3, 16, 12, 15
	rlwinm	r7, r7, 0, 0, 19
	oris	r3, r3, 0x1000
	mtspr	FSL_EIS_MAS0, r3
	oris	r4, r4, 0xc000
	rlwinm	r8, r8, 8, 20, 23
	or	r8, r4, r8
	mtspr	FSL_EIS_MAS1, r8
	or	r5, r7, r5
	mtspr	FSL_EIS_MAS2, r5
	or	r6, r7, r6
	mtspr	FSL_EIS_MAS3, r6
	li	r0, 0
	mtspr	FSL_EIS_MAS7, r0
	tlbwe
	sync
	isync
	blr

qoriq_tlb1_invalidate:
	rlwinm	r3, r3, 16, 12, 15
	oris	r3, r3, 0x1000
	mtspr	FSL_EIS_MAS0, r3
	li	r0, 0
	mtspr	FSL_EIS_MAS1, r0
	mtspr	FSL_EIS_MAS2, r0
	mtspr	FSL_EIS_MAS3, r0
	mtspr	FSL_EIS_MAS7, r0
	tlbwe
	sync
	isync
	blr

qoriq_tlb1_ts_0_only:
	mflr	r12
	li	r11, 16
	mtctr	r11
	li	r11, 0
2:
	rlwinm	r0, r11, 16, 12, 15
	oris	r0, r0, (FSL_EIS_MAS0_TLBSEL >> 16)
	mtspr	FSL_EIS_MAS0, r0
	tlbre
	mfspr	r0, FSL_EIS_MAS1
	andi.	r0, r0, FSL_EIS_MAS1_TS
	beq	1f
	mr	r3, r11
	bl	qoriq_tlb1_invalidate
1:
	addi	r11, r11, 1
	bdnz	2b
	mtlr	r12
	blr
