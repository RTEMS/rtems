/**
 * @file
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

#include <bspopts.h>
#include <bsp/linker-symbols.h>

/* External symbols */

.extern bsp_reset
.extern boot_card
.extern bsp_start_hook_0
.extern bsp_start_hook_1

/* Global symbols */

.globl start
.globl bsp_start_memcpy

/* Program Status Register definitions */

.equ PSR_MODE_USR,   0x10
.equ PSR_MODE_FIQ,   0x11
.equ PSR_MODE_IRQ,   0x12
.equ PSR_MODE_SVC,   0x13
.equ PSR_MODE_ABT,   0x17
.equ PSR_MODE_UNDEF, 0x1b
.equ PSR_MODE_SYS,   0x1f
.equ PSR_I,          0x80
.equ PSR_F,          0x40
.equ PSR_T,          0x20

.section ".bsp_start", "ax"

.arm

/*
 * This is the exception vector table and the pointers to the default
 * exceptions handlers.
 */

vector_block:

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
	.word	start
#endif

handler_addr_undef:

	.word	twiddle

handler_addr_swi:

	.word	twiddle

handler_addr_prefetch:

	.word	twiddle

handler_addr_abort:

	.word	twiddle

handler_addr_reserved:

	.word	twiddle

handler_addr_irq:

	.word	twiddle

handler_addr_fiq:

	.word	twiddle

/* Start entry */

start:

	/*
	 * We do not save the context since we do not return to the boot
	 * loader.
	 */

	/*
	 * Set SVC mode, disable interrupts and enable ARM instructions.
	 */
	mov	r0, #(PSR_MODE_SVC | PSR_I | PSR_F)
	msr	cpsr, r0

	/* Initialize stack pointer registers for the various modes */

	/* Enter IRQ mode and set up the IRQ stack pointer */
	mov	r0, #(PSR_MODE_IRQ | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_irq_end

	/* Enter FIQ mode and set up the FIQ stack pointer */
	mov	r0, #(PSR_MODE_FIQ | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_fiq_end

	/* Enter ABT mode and set up the ABT stack pointer */
	mov	r0, #(PSR_MODE_ABT | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_abt_end

	/* Enter UNDEF mode and set up the UNDEF stack pointer */
	mov	r0, #(PSR_MODE_UNDEF | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_undef_end

	/* Enter SVC mode and set up the SVC stack pointer */
	mov	r0, #(PSR_MODE_SVC | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	sp, =bsp_stack_svc_end

	/* Stay in SVC mode */

	/*
	 * Branch to start hook 0.
	 *
	 * This code up to the start hook 0 may run with an address offset so
	 * it must be position independent.  After the start hook 0 it is
	 * assumed that the code can run at its intended position.  The link
	 * register will be loaded with the absolute address.
	 */

	ldr	lr, =bsp_start_hook_0_done
	b	bsp_start_hook_0

bsp_start_hook_0_done:

	/*
	 * Initialize the exception vectors.  This includes the exceptions
	 * vectors and the pointers to the default exception handlers.
	 */

	ldr	r0, =bsp_section_vector_begin
	adr	r1, vector_block
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

	/* Branch to start hook 1 */
	bl	bsp_start_hook_1

	/* Branch to boot card */
	mov	r0, #0
#ifdef __thumb__
	ldr	r3, =boot_card
	mov	lr, pc
	bx      r3
.thumb
	bx	pc
	nop
.arm
#else
	bl	boot_card
#endif

	/* Branch to reset function */
#ifdef __thumb__
	ldr	r3, =bsp_reset
	mov	lr, pc
	bx      r3
.thumb
	bx	pc
	nop
.arm
#else
	bl	bsp_reset
#endif

	/* Spin forever */

twiddle:

	b	twiddle

DEFINE_FUNCTION_ARM(bsp_start_memcpy)

	/* Return if dest == src */
	cmp	r0, r1
	bxeq	lr

	/* Return if length is zero */
	mov	r3, #0
	cmp	r3, r2
	bxeq	lr

	/* Save non-volatile registers */
	push	{r4-r8, lr}

	/* Copy worker routine to stack */
	adr	ip, bsp_start_memcpy_begin
	ldm	ip, {r3-r8}
	push	{r3-r8}

	/* Execute worker routine */
	mov	r3, #0
	mov	ip, sp
	mov	lr, pc
	bx	ip

	/* Restore stack and non-volatile registers */
	add	sp, sp, #24
	pop	{r4-r8, lr}

	/* Return */
	bx	lr

bsp_start_memcpy_begin:

	/* Worker routine */
	ldr	ip, [r1, r3]
	str	ip, [r0, r3]
	add	r3, r3, #4
	cmp	r3, r2
	bcc	bsp_start_memcpy_begin
	bx	lr
