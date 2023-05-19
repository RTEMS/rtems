/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH & Co. KG
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

#include <rtems/asm.h>

#include <dev/cache/arm-data-cache-loop-set-way.h>

#if __ARM_ARCH >= 7 && (__ARM_ARCH_PROFILE == 'A' || __ARM_ARCH_PROFILE == 'R')
	.globl	rtems_cache_disable_data

	.syntax	unified
	.section	.text
	.arm

	/*
	 * This function disables the data cache on an ARMv7-AR compatible
	 * processor.
	 */
FUNCTION_ENTRY(rtems_cache_disable_data)
	/* Disable interrupts */
	mrs	r0, CPSR
	orr	r1, r0, #0x80
	msr	CPSR_fc, r1

	stmdb	sp!, {r4 - r11, lr}
	dmb

	/* Disable data cache in SCTLR */
	mrc	p15, 0, r1, c1, c0, 0
	bic	r1, r1, #0x4
	mcr	p15, 0, r1, c1, c0, 0
	isb

	/*
	 * Clean and invalidate the sets and ways of all data or unified cache
	 * levels using DCCISW (Data Cache line Clean and Invalidate by
	 * Set/Way).
	 */
	ARM_DATA_CACHE_LOOP_SET_WAY c14

	/* Restore interrupts */
	msr	CPSR_fc, r0

	ldmia	sp!, {r4 - r11, pc}
#endif
