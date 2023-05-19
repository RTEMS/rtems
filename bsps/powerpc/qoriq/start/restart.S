/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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
