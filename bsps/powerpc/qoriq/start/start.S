/**
 * @file
 *
 * @ingroup qoriq
 *
 * @brief BSP start.
 */

/*
 * Copyright (c) 2010, 2017 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>

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
#define FDT_REGISTER r17
#define CPU_SELF r18

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
	mr	FDT_REGISTER, r3
	bl	.Linitearly

	/* Get start stack */
	LA	START_STACK, _ISR_Stack_area_begin
	LA	r3, _ISR_Stack_size
	add	START_STACK, START_STACK, r3

	bl	.Linitmore

	/* Copy fast text */
	LA	r3, bsp_section_fast_text_begin
	LA	r4, bsp_section_fast_text_load_begin
	LA	r5, bsp_section_fast_text_size
	bl	.Lcopy
	LA	r3, bsp_section_fast_text_begin
	LA	r4, bsp_section_fast_text_size
	bl	rtems_cache_flush_multiple_data_lines
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* Copy read-only data */
	LA	r3, bsp_section_rodata_begin
	LA	r4, bsp_section_rodata_load_begin
	LA	r5, bsp_section_rodata_size
	bl	.Lcopy

	/* Copy FDT into read-only data */
	mr	r3, FDT_REGISTER
	bl	bsp_fdt_copy
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* Flush read-only data */
	LA	r3, bsp_section_rodata_begin
	LA	r4, bsp_section_rodata_size
	bl	rtems_cache_flush_multiple_data_lines
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* Copy fast data */
	LA	r3, bsp_section_fast_data_begin
	LA	r4, bsp_section_fast_data_load_begin
	LA	r5, bsp_section_fast_data_size
	bl	.Lcopy

	/* Copy data */
	LA	r3, bsp_section_data_begin
	LA	r4, bsp_section_data_load_begin
	LA	r5, bsp_section_data_size
	bl	.Lcopy

	/* NULL pointer access protection (only core 0 has to do this) */
	mfspr	r3, BOOKE_PIR
	cmpwi	r3, 0
	bne	.Lnull_area_setup_done
	LA	r3, bsp_section_start_begin
	SHIFT_RIGHT_IMMEDIATE	r3, r3, 2
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
	LA	r3, bsp_section_sbss_begin
	LA	r4, bsp_section_sbss_size
	bl	bsp_start_zero
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* Clear BSS */
	LA	r3, bsp_section_bss_begin
	LA	r4, bsp_section_bss_size
	bl	bsp_start_zero
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

#ifndef __powerpc64__
	/* Set up EABI and SYSV environment */
	bl	__eabi
#endif

	/* Clear command line */
	li	r3, 0

	bl	boot_card
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

.Lcopy:
	PPC_REG_CMP	r3, r4
	beqlr
	b	memcpy
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

.Linitearly:
#ifdef __powerpc64__
	/* Enable 64-bit computation mode for exceptions */
	mfspr	r0, BOOKE_EPCR
	oris	r0, r0, BOOKE_EPCR_ICM >> 16
	mtspr	BOOKE_EPCR, r0

	/* Enable 64-bit computation mode */
	mfmsr	r0
	oris	r0, r0, MSR_CM >> 16
	mtmsr	r0
	isync
#endif

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

#if defined(QORIQ_INITIAL_HID0) && !defined(QORIQ_IS_HYPERVISOR_GUEST)
	/* HID0 initialization */
	LWI	r0, QORIQ_INITIAL_HID0
	mtspr	HID0, r0
#endif

#ifdef __powerpc64__
	LA32	r2, .TOC.
#else
	/* Invalidate TLS anchor */
	li	r2, 0

	/* Set small-data anchor */
	LA	r13, _SDA_BASE_
#endif

	SET_SELF_CPU_CONTROL	CPU_SELF, r5

	blr

.Linitmore:
	mflr	SAVED_LINK_REGISTER

	/* Invalidate all TS1 MMU entries */
	li	r3, 1
	bl	qoriq_tlb1_invalidate_all_by_ts
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* Add TS1 entry for the first 4GiB of RAM */
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_M
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

	/* MSR initialization and use TS1 for address translation */
	LWI	INITIAL_MSR, QORIQ_INITIAL_MSR
	ori	r0, INITIAL_MSR, MSR_IS | MSR_DS
#ifdef QORIQ_IS_HYPERVISOR_GUEST
	oris	r0, r0, MSR_GS >> 16
#endif
	mtmsr	r0
	isync

	/*
	 * Initialize start stack.  The stacks are statically allocated and
	 * properly aligned.
	 */
	subi	r1, START_STACK, PPC_DEFAULT_CACHE_LINE_SIZE
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
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	mtmsr	INITIAL_MSR
	isync
	li	r3, SCRATCH_TLB
	bl	qoriq_tlb1_invalidate
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE

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
	PPC_REG_LOAD	r3, PER_CPU_INTERRUPT_STACK_HIGH(CPU_SELF)
	subi	r1, r3, PPC_MINIMUM_STACK_FRAME_SIZE
	CLEAR_RIGHT_IMMEDIATE	r1, r1, PPC_STACK_ALIGN_POWER
	li	r0, 0
	PPC_REG_STORE	r0, 0(r1)

#ifdef INITIALIZE_FPU
	bl	.Linitfpu
#endif

	mr	r3, CPU_SELF
	b	qoriq_start_thread
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif
_start_secondary_processor:
	bl	.Linitearly

	/* Get start stack */
	mr	START_STACK, r3

	bl	.Linitmore
	li	r3, 0
	bl	.Linitmmu
	mr	r3, CPU_SELF
	b	bsp_start_on_secondary_processor
	PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif /* RTEMS_SMP */

#ifdef __powerpc64__
#define START_NOP_FOR_LINKER_TOC_POINTER_RESTORE nop; nop; nop; nop
#else
#define START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif

	/* Exception vector prologues area */
	.section ".bsp_start_text", "ax"
	.align 4
bsp_exc_vector_base:
	/* Critical input */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 0
	b	ppc_exc_fatal_critical
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Machine check */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 1
	b	ppc_exc_fatal_machine_check
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Data storage */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 2
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Instruction storage */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 3
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* External input */
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	li	r3, 4
	b	ppc_exc_interrupt
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Alignment */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 5
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Program */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 6
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#ifdef __PPC_CPU_E6500__
	/* Floating-point unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 7
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif
	/* System call */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 8
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#ifdef __PPC_CPU_E6500__
	/* APU unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 9
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif
	/* Decrementer */
#ifdef QORIQ_IS_HYPERVISOR_GUEST
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	li	r3, 10
	b	ppc_exc_interrupt
#else
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 10
	b	ppc_exc_fatal_normal
#endif
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Fixed-interval timer interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 11
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Watchdog timer interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 12
	b	ppc_exc_fatal_critical
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Data TLB error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 13
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Instruction TLB error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 14
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Debug */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 15
	b	ppc_exc_fatal_debug
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* SPE APU unavailable or AltiVec unavailable */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 32
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* SPE floating-point data exception or AltiVec assist */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 33
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#ifndef __PPC_CPU_E6500__
	/* SPE floating-point round exception */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 34
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif
	/* Performance monitor */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 35
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#ifdef __PPC_CPU_E6500__
	/* Processor doorbell interrupt */
#if defined(QORIQ_IS_HYPERVISOR_GUEST) && defined(RTEMS_SMP)
	PPC_REG_STORE_UPDATE	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	PPC_REG_STORE	r3, PPC_EXC_GPR3_PROLOGUE_OFFSET(r1)
	li	r3, 36
	b	ppc_exc_interrupt
#else
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 36
	b	ppc_exc_fatal_normal
#endif
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Processor doorbell critical interrupt */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 37
	b	ppc_exc_fatal_critical
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Guest processor doorbell */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 38
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Guest processor doorbell critical and machine check */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 39
	b	ppc_exc_fatal_critical
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Hypervisor system call */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 40
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* Hypervisor privilege */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 41
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
	/* LRAT error */
	PPC_REG_STORE_UPDATE	r1, -EXC_GENERIC_SIZE(r1)
	PPC_REG_STORE	r3, GPR3_OFFSET(r1)
	li	r3, 42
	b	ppc_exc_fatal_normal
	START_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif

/* Symbol provided for debugging and tracing */
bsp_exc_vector_end:
