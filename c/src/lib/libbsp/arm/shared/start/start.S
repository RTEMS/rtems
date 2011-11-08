/**
 * @file
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#include <rtems/asm.h>
#include <rtems/system.h>	
#include <rtems/score/cpu.h>
	
#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>

	/* External symbols */
	.extern	bsp_reset
	.extern	boot_card
	.extern	bsp_start_hook_0
	.extern	bsp_start_hook_1

	/* Global symbols */
	.globl	_start
	.globl	bsp_start_vector_table_begin
	.globl	bsp_start_vector_table_end
	.globl	bsp_start_vector_table_size
	.globl	bsp_vector_table_size

	.section	".bsp_start_text", "ax"

#if defined(ARM_MULTILIB_ARCH_V4)

	.arm

/*
 * This is the exception vector table and the pointers to the default
 * exceptions handlers.
 */

bsp_start_vector_table_begin:

	ldr	pc, handler_addr_reset
	ldr	pc, handler_addr_undef
	ldr	pc, handler_addr_swi
	ldr	pc, handler_addr_prefetch
	ldr	pc, handler_addr_abort

	/* Program signature checked by boot loader */
	.word	0xb8a06f58

	ldr	pc, handler_addr_irq
	ldr	pc, handler_addr_fiq

handler_addr_reset:

#ifdef BSP_START_RESET_VECTOR
	.word	BSP_START_RESET_VECTOR
#else
	.word	_start
#endif

handler_addr_undef:

	.word	reset

handler_addr_swi:

	.word	reset

handler_addr_prefetch:

	.word	reset

handler_addr_abort:

	.word	reset

handler_addr_reserved:

	.word	reset

handler_addr_irq:

	.word	reset

handler_addr_fiq:

	.word	reset

bsp_start_vector_table_end:

/* Start entry */

_start:

	/*
	 * We do not save the context since we do not return to the boot
	 * loader.
	 */

	/*
	 * Set SVC mode, disable interrupts and enable ARM instructions.
	 */
	mov	r0, #(ARM_PSR_M_SVC | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0

	/* Initialize stack pointer registers for the various modes */

	/* Enter IRQ mode and set up the IRQ stack pointer */
	mov	r0, #(ARM_PSR_M_IRQ | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_irq_end

	/* Enter FIQ mode and set up the FIQ stack pointer */
	mov	r0, #(ARM_PSR_M_FIQ | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_fiq_end

	/* Enter ABT mode and set up the ABT stack pointer */
	mov	r0, #(ARM_PSR_M_ABT | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_abt_end

	/* Enter UND mode and set up the UND stack pointer */
	mov	r0, #(ARM_PSR_M_UND | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_und_end

	/* Enter SVC mode and set up the SVC stack pointer */
	mov	r0, #(ARM_PSR_M_SVC | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_svc_end

	/* Stay in SVC mode */

	/*
	 * Branch to start hook 0.
	 *
	 * The previous code and parts of the start hook 0 may run with an
	 * address offset.  This implies that only branches relative to the
	 * program counter are allowed.  After the start hook 0 it is assumed
	 * that the code can run at its intended position.  Thus the link
	 * register will be loaded with the absolute address.  In THUMB mode
	 * the start hook 0 must be within a 2kByte range due to the branch
	 * instruction limitation.
	 */

	ldr	lr, =bsp_start_hook_0_done
#ifdef __thumb__
	orr	lr, #1
#endif

	SWITCH_FROM_ARM_TO_THUMB	r0

	b	bsp_start_hook_0

bsp_start_hook_0_done:

	SWITCH_FROM_THUMB_TO_ARM

	/*
	 * Initialize the exception vectors.  This includes the exceptions
	 * vectors and the pointers to the default exception handlers.
	 */

	ldr	r0, =bsp_vector_table_begin
	adr	r1, bsp_start_vector_table_begin
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

	SWITCH_FROM_ARM_TO_THUMB	r0

	/* Branch to start hook 1 */
	bl	bsp_start_hook_1

	/* Branch to boot card */
	mov	r0, #0
	bl	boot_card

twiddle:

	/* Branch to reset function */
	bl	bsp_reset

	b	twiddle

.arm

reset:

	SWITCH_FROM_ARM_TO_THUMB	r0
	b	twiddle

#elif defined(ARM_MULTILIB_ARCH_V7M)

	.syntax	unified

	.extern	bsp_stack_main_end

	.thumb

bsp_start_vector_table_begin:

	.word	bsp_stack_main_end
	.word	_start /* Reset */
	.word	bsp_reset /* NMI */
	.word	bsp_reset /* Hard Fault */
	.word	bsp_reset /* MPU Fault */
	.word	bsp_reset /* Bus Fault */
	.word	bsp_reset /* Usage Fault */
	.word	bsp_reset /* Reserved */
	.word	bsp_reset /* Reserved */
	.word	bsp_reset /* Reserved */
	.word	bsp_reset /* Reserved */
	.word	bsp_reset /* SVC */
	.word	bsp_reset /* Debug Monitor */
	.word	bsp_reset /* Reserved */
	.word	bsp_reset /* PendSV */
	.word	bsp_reset /* SysTick */
	.rept	BSP_INTERRUPT_VECTOR_MAX + 1
	.word	bsp_reset /* IRQ */
	.endr

bsp_start_vector_table_end:

	.thumb_func

_start:

	ldr	sp, =bsp_stack_main_end
	ldr	lr, =bsp_start_hook_0_done + 1
	b	bsp_start_hook_0

bsp_start_hook_0_done:

	bl	bsp_start_hook_1
	movs	r0, #0
	bl	boot_card

twiddle:

	bl	bsp_reset
	b	twiddle

#endif /* defined(ARM_MULTILIB_ARCH_V7M) */

	.set	bsp_start_vector_table_size, bsp_start_vector_table_end - bsp_start_vector_table_begin
	.set	bsp_vector_table_size, bsp_start_vector_table_size
