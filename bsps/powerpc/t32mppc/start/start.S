/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/percpu.h>
#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

	.globl _start
	.globl bsp_exc_vector_base

	.section ".bsp_start_text", "ax"

	/* Primitive NULL pointer protection */
.rept 1024
	sc
.endr

_start:
	/* Enable time base */
	li	r0, 0x4000
	mtspr	HID0, r0

	/* Initialize start stack */
	LA	r1, _ISR_Stack_area_end
	subi	r1, r1, 16
	li	r0, 0
	stw	r0, 0(r1)

	SET_SELF_CPU_CONTROL	r3, r4

	/* Copy fast text */
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_load_begin
	LWI	r5, bsp_section_fast_text_size
	bl	copy

	/* Copy read-only data */
	LWI	r3, bsp_section_rodata_begin
	LWI	r4, bsp_section_rodata_load_begin
	LWI	r5, bsp_section_rodata_size
	bl	copy

	/* Copy fast data */
	LWI	r3, bsp_section_fast_data_begin
	LWI	r4, bsp_section_fast_data_load_begin
	LWI	r5, bsp_section_fast_data_size
	bl	copy

	/* Copy data */
	LWI	r3, bsp_section_data_begin
	LWI	r4, bsp_section_data_load_begin
	LWI	r5, bsp_section_data_size
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
	b	twiddle

copy:
	cmpw	r3, r4
	beqlr
	b	memcpy

	/* Exception vector prologues area */
	.section ".bsp_start_text", "ax"
	.align 4
bsp_exc_vector_base:
	/* Critical input */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 0
	b	ppc_exc_fatal_critical
	/* Machine check */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 1
	b	ppc_exc_fatal_machine_check
	/* Data storage */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 2
	b	ppc_exc_fatal_normal
	/* Instruction storage */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 3
	b	ppc_exc_fatal_normal
	/* External input */
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 4
	b	ppc_exc_interrupt
	/* Alignment */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 5
	b	ppc_exc_fatal_normal
	/* Program */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 6
	b	ppc_exc_fatal_normal
	/* Floating-point unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 7
	b	ppc_exc_fatal_normal
	/* System call */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 8
	b	ppc_exc_fatal_normal
	/* APU unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 9
	b	ppc_exc_fatal_normal
	/* Decrementer */
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	li	r3, 10
	b	ppc_exc_interrupt
	/* Fixed-interval timer interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 11
	b	ppc_exc_fatal_normal
	/* Watchdog timer interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 12
	b	ppc_exc_fatal_critical
	/* Data TLB error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 13
	b	ppc_exc_fatal_normal
	/* Instruction TLB error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 14
	b	ppc_exc_fatal_normal
	/* Debug */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 15
	b	ppc_exc_fatal_debug
	/* SPE APU unavailable or AltiVec unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 32
	b	ppc_exc_fatal_normal
	/* SPE floating-point data exception or AltiVec assist */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 33
	b	ppc_exc_fatal_normal
	/* SPE floating-point round exception */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 34
	b	ppc_exc_fatal_normal
	/* Performance monitor */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 35
	b	ppc_exc_fatal_normal
