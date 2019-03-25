/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief ARM data and prefetch abort exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

.extern _ARM_Exception_default

.globl _ARMV4_Exception_data_abort_set_handler
.globl _ARMV4_Exception_data_abort

.globl _ARMV4_Exception_prefetch_abort_set_handler
.globl _ARMV4_Exception_prefetch_abort

.section ".bss"

data_abort_handler:
.long 0

prefetch_abort_handler:
.long 0

.section ".text"

#ifdef __thumb__
	.thumb_func
#endif

_ARMV4_Exception_data_abort_set_handler:
	ldr	r1, =data_abort_handler
	str	r0, [r1]
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

#ifdef __thumb__
	.thumb_func
#endif

_ARMV4_Exception_prefetch_abort_set_handler:
	ldr	r1, =prefetch_abort_handler
	str	r0, [r1]
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

.arm

_ARMV4_Exception_prefetch_abort:

	/* Save context and load handler */
	sub	sp, #20
	stmdb	sp!, {r0-r12}
	mov	r4, #3
	ldr	r6, =prefetch_abort_handler

	b	save_more_context

_ARMV4_Exception_data_abort:

	/* Save context and load handler */
	sub	sp, #20
	stmdb	sp!, {r0-r12}
	mov	r4, #4
	ldr	r6, =data_abort_handler

save_more_context:

	/* Save more context */
	mov	r2, lr
	mrs	r3, spsr
	mrs	r7, cpsr
	orr	r5, r3, #ARM_PSR_I
	bic	r5, #ARM_PSR_T
	msr	cpsr, r5
	mov	r0, sp
	mov	r1, lr
	msr	cpsr, r7
	add	r5, sp, #72
	stmdb	r5!, {r0-r4}

	/* Call high level handler */
	ldr	r2, [r6]
	cmp	r2, #0
	ldreq	r2, =_ARM_Exception_default
	mov	r0, sp
#ifndef __thumb__
	mov	lr, pc
	mov	pc, r2
#else /* __thumb__ */
	SWITCH_FROM_ARM_TO_THUMB	r1
	bl	call_handler
	SWITCH_FROM_THUMB_TO_ARM
#endif /* __thumb__ */

	/* Restore context */
	ldmia	r5!, {r0-r4}
	mov	lr, r2
	msr	spsr, r3
	ldmia	sp!, {r0-r12}
	add	sp, #20

	/* Return from interrupt */
	subs	pc, lr, #8

#ifdef __thumb__
.thumb
call_handler:
	bx	r2
#endif /* __thumb__ */

#endif /* ARM_MULTILIB_ARCH_V4 */
