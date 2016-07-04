/**
 * @file
 *
 * @brief Boot and system start code.
 */

 /*
 * Copyright (c) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/system.h>
#include <rtems/score/cpu.h>

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>

	.extern	bsp_start_hyp_vector_table_begin
	.globl	bsp_start_arm_drop_hyp_mode
	.globl	bsp_arm_drop_hyp_mode_only

.arm

/*
 * The routine is called from startup code and it should
 * preserve all registers except r2 and r3. r0 can be used
 * as pass though argument in some cases, a1 is used for
 * CPU stack offset during startup and r4 to r6 to preserve
 * booloader arguments
 */

bsp_start_arm_drop_hyp_mode:
	ldr	r2, bsp_start_hyp_vector_table_begin_addr
	mcr	p15, 4, r2, c12, c0, 0

	mov	r2, #0
	mcr	p15, 4, r2, c1, c1, 0
	mcr	p15, 4, r2, c1, c1, 2
	mcr	p15, 4, r2, c1, c1, 3
/*
 * HSCTLR.TE
 * optional start of hypervisor handlers in Thumb mode
 *	orr	r0, #(1 << 30)
 */
	mcr	p15, 4, r2, c1, c0, 0	/* HSCTLR */
	mrc	p15, 4, r2, c1, c1, 1	/* HDCR */
	and	r2, #0x1f		/* Preserve HPMN */
	mcr	p15, 4, r2, c1, c1, 1	/* HDCR */

bsp_arm_drop_hyp_mode_only:
	/* Prepare SVC mode for eret */
	mrs	r2, cpsr
	bic	r2, r2, #ARM_PSR_M_MASK
	orr	r2, r2, #ARM_PSR_M_SVC
	msr	spsr_cxsf, r2

	adr	r2, 1f
	.inst 0xe12ef302	/* msr ELR_hyp, r2 */
	mov	r2, sp
	mov	r3, lr
	.inst 0xe160006e	/* eret */
1:	mov	sp, r2
	mov	lr, r3
	bx	lr

bsp_start_hyp_vector_table_begin_addr:
	.word	bsp_start_hyp_vector_table_begin
