/**
 * @file
 *
 * @ingroup ScoreCPU
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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/system.h>

#ifdef ARM_MULTILIB_ARCH_V4

.extern rtems_fatal_error_occurred

.globl arm_exc_data_abort_set_handler
.globl arm_exc_data_abort

.globl arm_exc_prefetch_abort_set_handler
.globl arm_exc_prefetch_abort

.section ".bss"

data_abort_handler:
.long 0

prefetch_abort_handler:
.long 0

.section ".text"

#ifdef __thumb__
	.thumb_func
#endif

arm_exc_data_abort_set_handler:
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

arm_exc_prefetch_abort_set_handler:
	ldr	r1, =prefetch_abort_handler
	str	r0, [r1]
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

.arm

arm_exc_prefetch_abort:

	/* Save context and load handler */
	sub	sp, #16
	stmdb	sp!, {r0-r12}
	ldr	r6, =prefetch_abort_handler

	b	save_more_context

arm_exc_data_abort:

	/* Save context and load handler */
	sub	sp, #16
	stmdb	sp!, {r0-r12}
	ldr	r6, =data_abort_handler

save_more_context:

	/* Save more context */
	mov	r2, lr
	mrs	r3, spsr
	mrs	r4, cpsr
	orr	r5, r3, #ARM_PSR_I
	bic	r5, #ARM_PSR_T
	msr	cpsr, r5
	mov	r0, sp
	mov	r1, lr
	msr	cpsr, r4
	add	r5, sp, #68
	stmdb	r5!, {r0-r3}

	/* Call high level handler */
	ldr	r2, [r6]
	cmp	r2, #0
	ldreq	r2, =rtems_fatal_error_occurred
	movne	r0, sp
	moveq	r0, #0xaa
#ifndef __thumb__
	mov	lr, pc
	mov	pc, r2
#else /* __thumb__ */
	SWITCH_FROM_ARM_TO_THUMB	r1
	bl	call_handler
	SWITCH_FROM_THUMB_TO_ARM
#endif /* __thumb__ */

	/* Restore context */
	ldmia	r5!, {r0-r3}
	mov	lr, r2
	msr	spsr, r3
	ldmia	sp!, {r0-r12}
	add	sp, #16

	/* Return from interrupt */
	subs	pc, lr, #8

#ifdef __thumb__
.thumb
call_handler:
	bx	r2
#endif /* __thumb__ */

#endif /* ARM_MULTILIB_ARCH_V4 */
