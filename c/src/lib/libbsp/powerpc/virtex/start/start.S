/*
 * Copyright (c) 2010-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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

	/* Start stack area */
.rept BSP_START_STACK_SIZE / 4
	b	twiddle
.endr

_start:

	/* Reset time base */
	li	r0, 0
	mtspr	TBWU, r0
	mtspr	TBWL, r0

	/* Initialize start stack */
	LWI	r1, _start
	stwu	r0, -4(r1)
	stwu	r0, -4(r1)

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
