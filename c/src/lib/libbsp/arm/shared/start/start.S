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

#include <bsp/linker-symbols.h>
#include <bsp/start.h>

/* External symbols */

.extern bsp_reset
.extern boot_card

/* Global symbols */

.globl start

.globl Reset_Handler
.globl Undefined_Handler
.globl SWI_Handler
.globl Prefetch_Handler
.globl Abort_Handler
.globl IRQ_Handler
.globl FIQ_Handler

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

/* Start entry */

.section ".entry"
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
	ldr	r1, =bsp_stack_irq_size
	ldr	sp, =bsp_stack_irq_start
	add	sp, sp, r1

        /* Enter FIQ mode and set up the FIQ stack pointer */
	mov	r0, #(PSR_MODE_FIQ | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_fiq_size
	ldr	sp, =bsp_stack_fiq_start
	add	sp, sp, r1

        /* Enter ABT mode and set up the ABT stack pointer */
	mov	r0, #(PSR_MODE_ABT | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_abt_size
	ldr	sp, =bsp_stack_abt_start
	add	sp, sp, r1

        /* Enter UNDEF mode and set up the UNDEF stack pointer */
	mov	r0, #(PSR_MODE_UNDEF | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_undef_size
	ldr	sp, =bsp_stack_undef_start
	add	sp, sp, r1

        /* Enter SVC mode and set up the SVC stack pointer */
	mov	r0, #(PSR_MODE_SVC | PSR_I | PSR_F)
	msr	cpsr, r0
	ldr	r1, =bsp_stack_svc_size
	ldr	sp, =bsp_stack_svc_start
	add	sp, sp, r1

        /* Stay in SVC mode */

        /* Brach to start hook 0 */
	bl	bsp_start_hook_0

        /*
	 * Initialize the exception vectors.  This includes the exceptions
	 * vectors and the pointers to the default exception handlers.
         */

	mov	r0, #0
	adr	r1, vector_block
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}
	ldmia	r1!, {r2-r9}
	stmia	r0!, {r2-r9}

        /* Brach to start hook 1 */
	bl	bsp_start_hook_1


        /* Brach to boot card */
	bl	boot_card

        /* Branch to reset function */
	bl	bsp_reset

        /* Spin forever */

twiddle:

	b	twiddle

/*
 * This is the exception vector table and the pointers to the default
 * exceptions handlers.
 */

vector_block:

	ldr	pc, Reset_Handler
	ldr	pc, Undefined_Handler
	ldr	pc, SWI_Handler
	ldr	pc, Prefetch_Handler
	ldr	pc, Abort_Handler
	nop
	ldr	pc, IRQ_Handler
	ldr	pc, FIQ_Handler

Reset_Handler:

	b	bsp_reset

Undefined_Handler:

	b	Undefined_Handler

SWI_Handler:

	b	SWI_Handler

Prefetch_Handler:

	b	Prefetch_Handler

Abort_Handler:

	b	Abort_Handler

	nop

IRQ_Handler:

	b	IRQ_Handler

FIQ_Handler:

	b	FIQ_Handler
