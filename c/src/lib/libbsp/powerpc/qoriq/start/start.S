/**
 * @file
 *
 * @ingroup qoriq
 *
 * @brief BSP start.
 */

/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
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

#define FIRST_TLB 0
#define SCRATCH_TLB QORIQ_TLB1_ENTRY_COUNT - 1
#define INITIAL_MSR r14

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
	bl	.Linit

#ifdef HAS_UBOOT
	bl	bsp_uboot_copy_board_info
#endif /* HAS_UBOOT */

	/* Initial MMU setup */
	bl	qoriq_tlb1_ts_0_only
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_I
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write

	/* MSR initialization */
	LWI	INITIAL_MSR, QORIQ_INITIAL_MSR
	ori	r0, INITIAL_MSR, MSR_IS | MSR_DS
	mtmsr	r0

#ifdef QORIQ_INITIAL_BUCSR
	/* BUCSR initialization */
	LWI	r0, QORIQ_INITIAL_BUCSR
	mtspr	FSL_EIS_BUCSR, r0
	isync
#endif

	/* Initialize start stack */
	LWI	r1, start_stack_end - PPC_MINIMUM_STACK_FRAME_SIZE
	clrrwi	r1, r1, PPC_STACK_ALIGN_POWER
	li	r0, 0
	stw	r0, 0(r1)

	/* Copy fast text */
	LWI	r3, bsp_section_fast_text_begin
	LWI	r4, bsp_section_fast_text_load_begin
	LWI	r5, bsp_section_fast_text_size
	bl	.Lcopy

	/* Copy read-only data */
	LWI	r3, bsp_section_rodata_begin
	LWI	r4, bsp_section_rodata_load_begin
	LWI	r5, bsp_section_rodata_size
	bl	.Lcopy

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

	/* Configure MMU */
	li	r3, FIRST_TLB
	li	r4, SCRATCH_TLB
	bl	qoriq_mmu_config
	mtmsr	INITIAL_MSR
	li	r3, SCRATCH_TLB
	bl	qoriq_tlb1_invalidate

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

	/* Do not use r3 here, since this could be the U-Boot board info */
.Linit:
	/* Reset time base */
	li	r0, 0
	mtspr	TBWU, r0
	mtspr	TBWL, r0

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

	/* Set small-data anchors */
	LA	r2, _SDA2_BASE_
	LA	r13, _SDA_BASE_

	blr

#ifdef RTEMS_SMP
#if QORIQ_THREAD_COUNT > 1
_start_thread:
	/* Adjust PIR */
	mfspr	r0, BOOKE_PIR
	srawi	r0, r0, 2
	ori	r0, r0, 1
	mtspr	BOOKE_PIR, r0

	bl	.Linit

	/* Initialize start stack */
	GET_SELF_CPU_CONTROL	r3
	lwz	r3, PER_CPU_INTERRUPT_STACK_HIGH(r3)
	subi	r1, r3, PPC_MINIMUM_STACK_FRAME_SIZE
	clrrwi	r1, r1, PPC_STACK_ALIGN_POWER
	li	r0, 0
	stw	r0, 0(r1)

	b	qoriq_start_thread
#endif
_start_secondary_processor:

	bl	.Linit

	/* Get start stack */
	mr	r1, r3

	/* Initial MMU setup */
	bl	qoriq_tlb1_ts_0_only
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_I
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 0
	li	r9, 11
	bl	qoriq_tlb1_write

	/* MSR initialization */
	LWI	INITIAL_MSR, QORIQ_INITIAL_MSR
	ori	r0, INITIAL_MSR, MSR_IS | MSR_DS
	mtmsr	r0

#ifdef QORIQ_INITIAL_BUCSR
	/* BUCSR initialization */
	LWI	r0, QORIQ_INITIAL_BUCSR
	mtspr	FSL_EIS_BUCSR, r0
	isync
#endif

	/* Initialize start stack */
	subi	r1, r1, PPC_MINIMUM_STACK_FRAME_SIZE
	clrrwi	r1, r1, PPC_STACK_ALIGN_POWER
	li	r0, 0
	stw	r0, 0(r1)

	/* Configure MMU */
	li	r3, FIRST_TLB
	li	r4, SCRATCH_TLB
	bl	qoriq_mmu_config
	mtmsr	INITIAL_MSR
	li	r3, SCRATCH_TLB
	bl	qoriq_tlb1_invalidate

	b	bsp_start_on_secondary_processor
#endif /* RTEMS_SMP */

	/* Exception vector prologues area */
	.section ".bsp_start_text", "ax"
	.align 4
bsp_exc_vector_base:
	stw	r1, ppc_exc_lock_crit@sdarel(r13)
	stw	r4, ppc_exc_vector_register_crit@sdarel(r13)
	li	r4, -32767
	b	ppc_exc_wrap_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 2
	b	ppc_exc_wrap_nopush_e500_mchk
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 3
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 4
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -PPC_EXC_MINIMAL_FRAME_SIZE(r1)
#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32763
#endif
	b	ppc_exc_wrap_async_normal
#ifdef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	nop
	nop
#endif
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 6
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 7
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 8
	b	ppc_exc_wrap_nopush_std
system_call:
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 12
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 24
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -PPC_EXC_MINIMAL_FRAME_SIZE(r1)
#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32752
#endif
	b	ppc_exc_wrap_async_normal
#ifdef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	nop
	nop
#endif
	stwu	r1, -PPC_EXC_MINIMAL_FRAME_SIZE(r1)
#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32749
#endif
	b	ppc_exc_wrap_async_normal
#ifdef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	nop
	nop
#endif
	stw	r1, ppc_exc_lock_crit@sdarel(r13)
	stw	r4, ppc_exc_vector_register_crit@sdarel(r13)
	li	r4, -32748
	b	ppc_exc_wrap_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 18
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 17
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 13
	b	ppc_exc_wrap_nopush_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 10
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 25
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 26
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 15
	b	ppc_exc_wrap_nopush_std

/* Symbol provided for debugging and tracing */
bsp_exc_vector_end:

	/* Start stack area */
	.section ".bsp_rwextra", "aw", @nobits
	.align 4
	.space 4096
start_stack_end:
