/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQMMU
 *
 * @brief qoriq_tlb1_write() and qoriq_tlb1_invalidate() implementation.
 */

/*
 * Copyright (C) 2011, 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
