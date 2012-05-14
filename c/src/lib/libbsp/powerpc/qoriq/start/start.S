/**
 * @file
 *
 * @ingroup qoriq
 *
 * @brief BSP start.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/vectors.h>

#define FIRST_TLB 0
#define SCRATCH_TLB 15
#define INITIAL_MSR r14
#define UBOOT_BOARD_INFO r15

	.globl _start
	.globl bsp_exc_vector_base

	.section ".bsp_start_text", "ax"

_start:
	/* Reset time base */
	li	r0, 0
	mtspr	TBWU, r0
	mtspr	TBWL, r0

#ifdef HAS_UBOOT
	mr	UBOOT_BOARD_INFO, r3
#endif /* HAS_UBOOT */

	/* Initial MMU setup */
	bl	qoriq_tlb1_ts_0_only
	li	r3, SCRATCH_TLB
	li	r4, FSL_EIS_MAS1_TS
	li	r5, FSL_EIS_MAS2_I
	li	r6, FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW | FSL_EIS_MAS3_SX
	li	r7, 0
	li	r8, 11
	bl	qoriq_tlb1_write

	/* MSR initialization */
	LWI	INITIAL_MSR, QORIQ_INITIAL_MSR
	ori	r0, INITIAL_MSR, MSR_IS | MSR_DS
	mtmsr	r0

	/* SPEFSCR initialization */
	LWI	r0, QORIQ_INITIAL_SPEFSCR
	mtspr	FSL_EIS_SPEFSCR, r0

	/* Initialize start stack */
	LWI	r1, start_stack_end
	subi	r1, r1, 16
	li	r0, 0
	stw	r0, 0(r1)

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

	/* NULL pointer access protection (only core 0 has to do this) */
	mfspr	r3, BOOKE_PIR
	cmpwi	r3, 0
	bne	null_area_setup_done
	LWI	r3, bsp_section_start_begin
	srawi	r3, r3, 2
	mtctr	r3
	li	r3, -4
	LWI	r4, 0x44000002
null_area_setup_loop:
	stwu	r4, 4(r3)
	bdnz	null_area_setup_loop
null_area_setup_done:

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

#ifdef HAS_UBOOT
	li	r3, SCRATCH_TLB
	li	r4, 0
	li	r5, 0
	li	r6, FSL_EIS_MAS3_SR
	mr	r7, UBOOT_BOARD_INFO
	li	r8, 1
	bl	qoriq_tlb1_write
	mr	r3, UBOOT_BOARD_INFO
	bl	bsp_uboot_copy_board_info
	li	r3, SCRATCH_TLB
	bl	qoriq_tlb1_invalidate
#endif /* HAS_UBOOT */

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
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32763
	b	ppc_exc_wrap_async_normal
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
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32752
	b	ppc_exc_wrap_async_normal
	stwu	r1, -PPC_EXC_MINIMAL_FRAME_SIZE(r1)
	stw	r4, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	r4, -32749
	b	ppc_exc_wrap_async_normal
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

	/* Start stack area */
	.section ".bsp_rwextra", "aw", @nobits
	.align 4
	.space 4096
start_stack_end:
