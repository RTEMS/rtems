/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2010, 2013 embedded brains GmbH & Co. KG
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

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

	.globl _start
	.globl virtex_exc_vector_base

	.section ".bsp_start_text", "ax"

virtex_exc_vector_base:

	b	_start

	/* Critical Input 0x0100 */
	/* Machine Check 0x0200 */
	/* Data Storage 0x0300 */
	/* Instruction Storage 0x0400 */
	/* External 0x0500 */
	/* Alignment 0x0600 */
	/* Program 0x0700 */
	/* FPU Unavailable 0x0800 */
	/* System Call 0x0C00 */
	/* APU Unavailable 0x0F20 */
	/* Programmable-Interval Timer 0x1000 */
	/* Fixed-Interval Timer 0x1010 */
	/* Watchdog Timer 0x1020 */
	/* Data TLB Miss 0x1100 */
	/* Instruction TLB Miss 0x1200 */
	/* Debug 0x2000 */
.rept 0x2000 / 4 - 1
	b	twiddle
.endr

_start:

	/* Reset time base */
	li	r0, 0
	mtspr	TBWU, r0
	mtspr	TBWL, r0

	/*
	 * Initialize start stack.  The stacks are statically allocated and
	 * properly aligned.
	 */
	LA	r1, _ISR_Stack_area_end
	subi	r1, r1, PPC_DEFAULT_CACHE_LINE_SIZE
	stw	r0, 0(r1)

	/* Copy fast text */
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_load_begin
	LWI	r5, bsp_section_fast_text_size
	bl	copy

	/* Copy fast data */
	LWI	r3, bsp_section_fast_data_begin
	LWI	r4, bsp_section_fast_data_load_begin
	LWI	r5, bsp_section_fast_data_size
	bl	copy

	/* Clear SBSS */
	LWI	r3, bsp_section_sbss_begin
	LWI	r4, bsp_section_sbss_size
	bl	bsp_start_zero

	/* Clear BSS */
	LWI	r3, bsp_section_bss_begin
	LWI	r4, bsp_section_bss_size
	bl	bsp_start_zero

	/* Set up EABI and SYSV environment */
	bl	__eabi

	/* Clear command line */
	li	r3, 0

	bl	boot_card

twiddle:

	bl	bsp_reset
	b	twiddle

copy:

	cmpw	r3, r4
	beqlr
	b	memcpy

	/* Reset entry */
	.section ".virtex_reset", "ax"

jump_to_start:

	LWI	r3, _start
	mtctr	r3
	bctr
	b	jump_to_start
