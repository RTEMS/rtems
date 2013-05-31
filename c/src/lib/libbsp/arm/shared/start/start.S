/**
 * @file
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
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
	.extern bsp_stack_irq_end
	.extern bsp_stack_fiq_end
	.extern bsp_stack_abt_end
	.extern bsp_stack_und_end
	.extern bsp_stack_svc_end
#ifdef RTEMS_SMP
	.extern bsp_stack_all_size
#endif
	.extern	_ARMV4_Exception_undef_default
	.extern	_ARMV4_Exception_swi_default
	.extern	_ARMV4_Exception_data_abort_default
	.extern	_ARMV4_Exception_pref_abort_default
	.extern	_ARMV4_Exception_reserved_default
	.extern	_ARMV4_Exception_irq_default
	.extern	_ARMV4_Exception_fiq_default
	.extern	_ARMV7M_Exception_default

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

	.word	_ARMV4_Exception_undef_default

handler_addr_swi:

	.word	_ARMV4_Exception_swi_default

handler_addr_prefetch:

	.word	_ARMV4_Exception_data_abort_default

handler_addr_abort:

	.word	_ARMV4_Exception_pref_abort_default

handler_addr_reserved:

	.word	_ARMV4_Exception_reserved_default

handler_addr_irq:

	.word	_ARMV4_Exception_irq_default

handler_addr_fiq:

	.word	_ARMV4_Exception_fiq_default

bsp_start_vector_table_end:

/* Start entry */

_start:

	/*
	 * We do not save the context since we do not return to the boot
	 * loader.
	 */

#ifdef RTEMS_SMP
	/* Read MPIDR */
	mrc	p15, 0, r0, c0, c0, 5

	/* Calculate stack offset */
	and	r0, #0xff
	ldr	r1, =bsp_stack_all_size
	mul	r1, r0
#endif

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
#ifdef RTEMS_SMP
	add	sp, r1
#endif

	/* Enter FIQ mode and set up the FIQ stack pointer */
	mov	r0, #(ARM_PSR_M_FIQ | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_fiq_end
#ifdef RTEMS_SMP
	add	sp, r1
#endif

	/* Enter ABT mode and set up the ABT stack pointer */
	mov	r0, #(ARM_PSR_M_ABT | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_abt_end
#ifdef RTEMS_SMP
	add	sp, r1
#endif

	/* Enter UND mode and set up the UND stack pointer */
	mov	r0, #(ARM_PSR_M_UND | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_und_end
#ifdef RTEMS_SMP
	add	sp, r1
#endif

	/* Enter SVC mode and set up the SVC stack pointer */
	mov	r0, #(ARM_PSR_M_SVC | ARM_PSR_I | ARM_PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_svc_end
#ifdef RTEMS_SMP
	add	sp, r1
#endif

	/* Stay in SVC mode */

#ifdef ARM_MULTILIB_VFP_D32
	/* Read CPACR */
	mrc p15, 0, r0, c1, c0, 2

	/* Enable CP10 and CP11 */
	orr r0, r0, #(1 << 20)
	orr r0, r0, #(1 << 22)

	/* Clear ASEDIS and D32DIS */
	bic r0, r0, #(3 << 30)

	/* Write CPACR */
	mcr p15, 0, r0, c1, c0, 2
	isb

	/* Enable FPU */
	mov r0, #(1 << 30)
	vmsr FPEXC, r0
#endif

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
	cmp	r0, r1
	beq	bsp_vector_table_copy_done
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

bsp_vector_table_copy_done:

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

#elif defined(ARM_MULTILIB_ARCH_V7M)

	.syntax	unified

	.extern	bsp_stack_main_end

	.thumb

bsp_start_vector_table_begin:

	.word	bsp_stack_main_end
	.word	_start /* Reset */
	.word	_ARMV7M_Exception_default /* NMI */
	.word	_ARMV7M_Exception_default /* Hard Fault */
	.word	_ARMV7M_Exception_default /* MPU Fault */
	.word	_ARMV7M_Exception_default /* Bus Fault */
	.word	_ARMV7M_Exception_default /* Usage Fault */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* SVC */
	.word	_ARMV7M_Exception_default /* Debug Monitor */
	.word	_ARMV7M_Exception_default /* Reserved */
	.word	_ARMV7M_Exception_default /* PendSV */
	.word	_ARMV7M_Exception_default /* SysTick */
	.rept	BSP_INTERRUPT_VECTOR_MAX + 1
	.word	_ARMV7M_Exception_default /* IRQ */
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
