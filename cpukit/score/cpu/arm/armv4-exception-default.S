/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/system.h>

#ifdef ARM_MULTILIB_ARCH_V4

#define MORE_CONTEXT_SIZE \
  (ARM_EXCEPTION_FRAME_SIZE - ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET)

.extern _ARM_Exception_default

.globl _ARMV4_Exception_undef_default
.globl _ARMV4_Exception_swi_default
.globl _ARMV4_Exception_data_abort_default
.globl _ARMV4_Exception_pref_abort_default
.globl _ARMV4_Exception_reserved_default
.globl _ARMV4_Exception_irq_default
.globl _ARMV4_Exception_fiq_default

.section ".text"

.arm

_ARMV4_Exception_undef_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #1

	b	save_more_context

_ARMV4_Exception_swi_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #2

	b	save_more_context

_ARMV4_Exception_pref_abort_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #3

	b	save_more_context

_ARMV4_Exception_data_abort_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #4

	b	save_more_context

_ARMV4_Exception_reserved_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #5

	b	save_more_context

_ARMV4_Exception_irq_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #6

	b	save_more_context

_ARMV4_Exception_fiq_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #7

save_more_context:

	/* Save more context */
	mov	r2, lr
	mrs	r3, spsr
	mrs	r7, cpsr
	orr	r5, r3, #ARM_PSR_I
	bic	r5, #ARM_PSR_T
	msr	cpsr, r5
	sub	r0, sp, #ARM_EXCEPTION_FRAME_SIZE
	mov	r1, lr
	msr	cpsr, r7
	mov	r5, #0
	add	r6, sp, #ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET
	stm	r6, {r0-r5}

	/* Argument for high level handler */
	mov	r0, sp

#ifdef ARM_MULTILIB_VFP_D32
	/* Ensure that the FPU is enabled */
	vmrs	r1, FPEXC
	tst	r1, #(1 << 30)
	beq	fpu_save_done

	add	r3, sp, #ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET
	sub	sp, #(ARM_VFP_CONTEXT_SIZE + 4)
	add	r4, sp, #4
	bic	r4, r4, #7
	str	r4, [r3]
	vmrs	r2, FPSCR
	stmia	r4!, {r1-r2}
	vstmia	r4!, {d0-d15}
	vstmia	r4!, {d16-d31}

fpu_save_done:
#endif

	/* Call high level handler */
	SWITCH_FROM_ARM_TO_THUMB	r1
	bl	_ARM_Exception_default

	/* Just in case */
twiddle:
	b	twiddle

#endif /* ARM_MULTILIB_ARCH_V4 */
