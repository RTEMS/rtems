/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2024 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

.section ".text"

.arm

FUNCTION_ENTRY(_CPU_Exception_resume)

#ifdef ARM_MULTILIB_VFP
	ldr	r1, [r0, #ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET]
	cmp	r1, #0
	beq	.Lvfp_restore_done

	/* Restore FPEXC, FPSCR, and D0-D31 */
	ldmia	r1!, {r2-r3}
	vmsr	FPEXC, r2
	vmsr	FPSCR, r3
	vstmia	r1!, {d0-d15}
#ifdef ARM_MULTILIB_VFP_D32
	vstmia	r1!, {d16-d31}
#endif

.Lvfp_restore_done:
#endif /* ARM_MULTILIB_VFP */

	/*
	 * Restore the original stack pointer of the exception mode.  Assume
	 * that the exception frame was produced by a default exception
	 * handler.
	 */
	mov	sp, r0

	ldr	r1, [r0, #ARM_EXCEPTION_FRAME_REGISTER_PC_OFFSET]
	ldr	r2, [r0, #ARM_EXCEPTION_FRAME_REGISTER_CPSR_OFFSET]
	mov	lr, r1
	msr	spsr, r2
	mrs	r3, cpsr
	bic	r4, r2, #(ARM_PSR_I | ARM_PSR_F)
	and	r5, r3, #(ARM_PSR_I | ARM_PSR_F)
	orr	r4, r4, r5

	/* We assume that we do not resume to user mode */
	msr	cpsr, r4

	/* Restore potentially banked registers in the mode to resume */
	add	r1, r0, #ARM_EXCEPTION_FRAME_REGISTER_R8_OFFSET
	ldm	r1, {r8-r13}

	msr	cpsr, r3
	ldm	r0, {r0-r7}
	movs	pc, lr

FUNCTION_END(_CPU_Exception_resume)

#endif /* ARM_MULTILIB_ARCH_V4 */
