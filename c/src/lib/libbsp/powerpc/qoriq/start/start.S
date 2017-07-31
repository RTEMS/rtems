/**
 * @file
 *
 * @ingroup qoriq
 *
 * @brief BSP start.
 */

/*
 * Copyright (c) 2010, 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/percpu.h>

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/vectors.h>

#if (QORIQ_INITIAL_MSR & MSR_FP) != 0
#define INITIALIZE_FPU
#endif

#define FIRST_TLB 0
#define SCRATCH_TLB QORIQ_TLB1_ENTRY_COUNT - 1
#define INITIAL_MSR r14
#define START_STACK r15
#define SAVED_LINK_REGISTER r16

	.globl _start
#ifdef RTEMS_SMP
#if QORIQ_THREAD_COUNT > 1
	.globl _start_thread
#endif
	.globl _start_secondary_processor
#endif
	.globl bsp_exc_vector_base

	.section ".bsp_start_text", "ax"

_start:
	bl	.Linitearly
	bl	bsp_fdt_copy

	/* Get start stack */
	LWI	START_STACK, start_stack_end

	bl	.Linitmore

	/* Copy fast text */
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_load_begin
	LWI	r5, bsp_section_fast_text_size
	bl	.Lcopy
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_size
	bl	rtems_cache_flush_multiple_data_lines

	/* Copy read-only data */
	LWI	r3, bsp_section_rodata_begin
	LWI	r4, bsp_section_rodata_load_begin
	LWI	r5, bsp_section_rodata_size
	bl	.Lcopy
	LWI	r3, bsp_section_rodata_begin
	LWI	r4, bsp_section_rodata_size
	bl	rtems_cache_flush_multiple_data_lines

	/* Copy fast data */
	LWI	r3, bsp_section_fast_data_begin
	LWI	r4, bsp_section_fast_data_load_begin
	LWI	r5, bsp_section_fast_data_size
	bl	.Lcopy

	/* Copy data */
	LWI	r3, bsp_section_data_begin
	LWI	r4, bsp_section_data_load_begin
	LWI	r5, bsp_section_data_size
	bl	.Lcopy

	/* NULL pointer access protection (only core 0 has to do this) */
	mfspr	r3, BOOKE_PIR
	cmpwi	r3, 0
	bne	.Lnull_area_setup_done
	LWI	r3, bsp_section_start_begin
	srawi	r3, r3, 2
	mtctr	r3
	li	r3, -4
	LWI	r4, 0x44000002
.Lnull_area_setup_loop:
	stwu	r4, 4(r3)
	bdnz	.Lnull_area_setup_loop
.Lnull_area_setup_done:

	li	r3, 1
	bl	.Linitmmu

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

.Lcopy:
	cmpw	r3, r4
	beqlr
	b	memcpy

.Linitearly:
	/*
	 * Do not use r3 here, since it holds the FDT base pointer provided by
	 * the boot loader.
	 */

	/* Disable decrementer */
	mfspr	r0, BOOKE_TCR
	LWI	r4, BOOKE_TCR_DIE
	andc	r0, r0, r4
	mtspr	BOOKE_TCR, r0

#ifdef QORIQ_INITIAL_SPEFSCR
	/* SPEFSCR initialization */
	LWI	r0, QORIQ_INITIAL_SPEFSCR
	mtspr	FSL_EIS_SPEFSCR, r0
#endif

#ifdef QORIQ_INITIAL_BUCSR
	/* BUCSR initialization */
	LWI	r0, QORIQ_INITIAL_BUCSR
	mtspr	FSL_EIS_BUCSR, r0
	isync
#endif

#ifdef QORIQ_INITIAL_HID0
	/* HID0 initialization */
	LWI	r0, QORIQ_INITIAL_HID0
	mtspr	HID0, r0
#endif

	/* Invalidate TLS anchor */
	li	r2, 0

	/* Set small-data anchor */
	LA	r13, _SDA_BASE_

	SET_SELF_CPU_CONTROL	r4, r5

	blr

.Linitmore:
	mflr	SAVED_LINK_REGISTER

	/* Invalidate all TS1 MMU entries */
	li	r3, 1
	bl	qoriq_tlb1_invalidate_all_by_ts

	/* Add TS1 entry for the first 4GiB of RAM */
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_M
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write

	/* MSR initialization and use TS1 for address translation */
	LWI	INITIAL_MSR, QORIQ_INITIAL_MSR
	ori	r0, INITIAL_MSR, MSR_IS | MSR_DS
	mtmsr	r0
	isync

	/*
	 * Initialize start stack.  Make sure that we do not share a cache line
	 * with the heap block management, since initial stacks for the
	 * secondary processors are allocated from the workspace.
	 */
	subi	r1, START_STACK, 2 * PPC_DEFAULT_CACHE_LINE_SIZE
	clrrwi	r1, r1, PPC_DEFAULT_CACHE_LINE_POWER
	li	r0, 0
	PPC_REG_STORE	r0, 0(r1)

#ifdef INITIALIZE_FPU
	bl	.Linitfpu
#endif

	mtlr	SAVED_LINK_REGISTER
	blr

.Linitmmu:
	mflr	SAVED_LINK_REGISTER

	/* Configure MMU */
	li	r4, FIRST_TLB
	li	r5, SCRATCH_TLB
	bl	qoriq_mmu_config
	mtmsr	INITIAL_MSR
	isync
	li	r3, SCRATCH_TLB
	bl	qoriq_tlb1_invalidate

	mtlr	SAVED_LINK_REGISTER
	blr

#ifdef INITIALIZE_FPU
	/*
	 * Write a value to the FPRs to initialize the hidden tag bits.  See
	 * also "Core Software Initialization Requirements" of the e500mc
	 * reference manual for example.
	 */
.Linitfpu:
	li	r0, 0
	stw	r0, 0(r1)
	stw	r0, 4(r1)
	lfd	f0, 0(r1)
	fmr	f1, f0
	fmr	f2, f0
	fmr	f3, f0
	fmr	f4, f0
	fmr	f5, f0
	fmr	f6, f0
	fmr	f7, f0
	fmr	f8, f0
	fmr	f9, f0
	fmr	f10, f0
	fmr	f11, f0
	fmr	f12, f0
	fmr	f13, f0
	fmr	f14, f0
	fmr	f15, f0
	fmr	f16, f0
	fmr	f17, f0
	fmr	f18, f0
	fmr	f19, f0
	fmr	f20, f0
	fmr	f21, f0
	fmr	f22, f0
	fmr	f23, f0
	fmr	f24, f0
	fmr	f25, f0
	fmr	f26, f0
	fmr	f27, f0
	fmr	f28, f0
	fmr	f29, f0
	fmr	f30, f0
	fmr	f31, f0
	blr
#endif

#ifdef RTEMS_SMP
#if QORIQ_THREAD_COUNT > 1
_start_thread:
	/* Adjust PIR */
	mfspr	r0, BOOKE_PIR
	srawi	r0, r0, 2
	ori	r0, r0, 1
	mtspr	BOOKE_PIR, r0

	bl	.Linitearly

	/* Initialize start stack */
	GET_SELF_CPU_CONTROL	r3
	lwz	r3, PER_CPU_INTERRUPT_STACK_HIGH(r3)
	subi	r1, r3, PPC_MINIMUM_STACK_FRAME_SIZE
	clrrwi	r1, r1, PPC_STACK_ALIGN_POWER
	li	r0, 0
	PPC_REG_STORE	r0, 0(r1)

#ifdef INITIALIZE_FPU
	bl	.Linitfpu
#endif

	b	qoriq_start_thread
#endif
_start_secondary_processor:
	bl	.Linitearly

	/* Get start stack */
	mr	START_STACK, r3

	bl	.Linitmore
	li	r3, 0
	bl	.Linitmmu
	b	bsp_start_on_secondary_processor
#endif /* RTEMS_SMP */

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
	b	ppc_exc_interrupt
	nop
	nop
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
#ifdef __PPC_CPU_E6500__
	/* Floating-point unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 7
	b	ppc_exc_fatal_normal
#endif
	/* System call */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 8
	b	ppc_exc_fatal_normal
#ifdef __PPC_CPU_E6500__
	/* APU unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 9
	b	ppc_exc_fatal_normal
#endif
	/* Decrementer */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 10
	b	ppc_exc_fatal_normal
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
#ifndef __PPC_CPU_E6500__
	/* SPE floating-point round exception */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 34
	b	ppc_exc_fatal_normal
#endif
	/* Performance monitor */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 35
	b	ppc_exc_fatal_normal
#ifdef __PPC_CPU_E6500__
	/* Processor doorbell interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 36
	b	ppc_exc_fatal_normal
	/* Processor doorbell critical interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 37
	b	ppc_exc_fatal_critical
	/* Guest processor doorbell */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 38
	b	ppc_exc_fatal_normal
	/* Guest processor doorbell critical and machine check */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 39
	b	ppc_exc_fatal_critical
	/* Hypervisor system call */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 40
	b	ppc_exc_fatal_normal
	/* Hypervisor privilege */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 41
	b	ppc_exc_fatal_normal
	/* LRAT error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 42
	b	ppc_exc_fatal_normal
#endif

/* Symbol provided for debugging and tracing */
bsp_exc_vector_end:

	/* Start stack area */
	.section ".bsp_rwextra", "aw", @nobits
	.align 4
	.space 4096
start_stack_end:
