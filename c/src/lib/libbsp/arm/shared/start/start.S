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
.globl SWI_Handler

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

.section ".entry"

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

	.word	start

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

	ldr	r0, =bsp_section_vector_start
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

SWI_Handler:

twiddle:

	b	twiddle
