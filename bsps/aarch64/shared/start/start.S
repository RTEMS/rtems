/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Shared
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

#include <bspopts.h>

	/* Global symbols */
	.globl	_start
	.section	".bsp_start_text", "ax"

/* Start entry */

_start:

	/*
	 * We do not save the context since we do not return to the boot
	 * loader but preserve x1 and x2 to allow access to bootloader parameters
	 */
#ifndef BSP_START_NEEDS_REGISTER_INITIALIZATION
	mov	x5, x1		/* machine type number or ~0 for DT boot */
	mov	x6, x2		/* physical address of ATAGs or DTB */
#else /* BSP_START_NEEDS_REGISTER_INITIALIZATION */
	mov	x0, XZR
	mov	x1, XZR
	mov	x2, XZR
	mov	x3, XZR
	mov	x4, XZR
	mov	x5, XZR
	mov	x6, XZR
	mov	x7, XZR
	mov	x8, XZR
	mov	x9, XZR
	mov	x10, XZR
	mov	x11, XZR
	mov	x12, XZR
	mov	x13, XZR
	mov	x14, XZR
	mov	x15, XZR
	mov	x16, XZR
	mov	x17, XZR
	mov	x18, XZR
	mov	x19, XZR
	mov	x20, XZR
	mov	x21, XZR
	mov	x22, XZR
	mov	x23, XZR
	mov	x24, XZR
	mov	x25, XZR
	mov	x26, XZR
	mov	x27, XZR
	mov	x28, XZR
	mov	x29, XZR
	mov	x30, XZR
#ifdef AARCH64_MULTILIB_VFP
#endif
#endif

	/* Initialize SCTLR_EL1 */
	mov x0, XZR
#if defined(RTEMS_DEBUG)
	/* Enable Stack alignment checking */
	orr x0, x0, #(1<<3)
#endif
	msr SCTLR_EL1, x0

#ifdef BSP_START_IN_HYP_SUPPORT
	/* Drop from EL2 to EL1 */

	/* Configure HCR_EL2 */
	mrs x0, HCR_EL2
	/* Set EL1 Execution state to AArch64 */
	orr x0, x0, #(1<<31)
	/* Disable ID traps */
	bic x0, x0, #(1<<15)
	bic x0, x0, #(1<<16)
	bic x0, x0, #(1<<17)
	bic x0, x0, #(1<<18)
	msr HCR_EL2, x0

	/* Set to EL1h mode for eret */
	mov x0, #0b00101
	msr SPSR_EL2, x0

	/* Set EL1 entry point */
	adr x0, _el1_start
	msr ELR_EL2, x0
	eret
_el1_start:
#endif

#ifdef RTEMS_SMP
	/* Read MPIDR and get current processor index */
        mrs	x7, mpidr_el1
	and	x7, #0xff
#endif

#ifdef RTEMS_SMP
        /*
         * Get current per-CPU control and store it in PL1 only Thread ID
         * Register (TPIDRPRW).
         */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	ldr	w1, =_Per_CPU_Information
#else
	ldr	x1, =_Per_CPU_Information
#endif
	add	x1, x1, x7, asl #PER_CPU_CONTROL_SIZE_LOG2
	mcr	p15, 0, x1, c13, c0, 4

#endif

	/* Calculate interrupt stack area end for current processor */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	ldr	w1, =_ISR_Stack_size
#else
	ldr	x1, =_ISR_Stack_size
#endif
#ifdef RTEMS_SMP
	add	x3, x7, #1
	mul	x1, x1, x3
#endif
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	ldr	w2, =_ISR_Stack_area_begin
#else
	ldr	x2, =_ISR_Stack_area_begin
#endif
	add	x3, x1, x2

	/* Save original DAIF value */
	mrs	x4, DAIF

#ifdef BSP_START_NEEDS_REGISTER_INITIALIZATION
	mov	x8, XZR
	mov	x9, XZR
	mov	x10, XZR
	mov	x11, XZR
	mov	x12, XZR
	mov	x13, XZR
	mov	x14, XZR
	mov	x15, XZR
#endif

	/*
	 * SPx: the stack pointer corresponding to the current exception level
	 * Normal operation for RTEMS on AArch64 uses SPx and runs on EL1
	 * Exception operation (synchronous errors, IRQ, FIQ, System Errors) uses SP0
	*/
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	ldr	w1, =bsp_stack_exception_size
#else
	ldr	x1, =bsp_stack_exception_size
#endif
	/* Switch to SP0 and set exception stack */
	msr	spsel, #0
	mov	sp, x3
	/* Switch back to SPx for normal operation */
	msr	spsel, #1
	sub	x3, x3, x1

	/* Set SP1 stack used for normal operation */
	mov	sp, x3

	/* Stay in EL1 mode */

#ifdef AARCH64_MULTILIB_VFP
#ifdef AARCH64_MULTILIB_HAS_CPACR
	/* Read CPACR */
	mrs x0, CPACR_EL1

	/* Enable EL1 access permissions for CP10 */
	orr x0, x0, #(1 << 20)

	/* Write CPACR */
	msr CPACR_EL1, x0
	isb
#endif

	/* FPU does not need to be enabled on AArch64 */

#ifdef BSP_START_NEEDS_REGISTER_INITIALIZATION
	mov	x0, #0
	mov	CPTR_EL3, XZR
	mov	CPTR_EL2, XZR
	mov	d0, XZR
	mov	d1, XZR
	mov	d2, XZR
	mov	d3, XZR
	mov	d4, XZR
	mov	d5, XZR
	mov	d6, XZR
	mov	d7, XZR
	mov	d8, XZR
	mov	d9, XZR
	mov	d10, XZR
	mov	d11, XZR
	mov	d12, XZR
	mov	d13, XZR
	mov	d14, XZR
	mov	d15, XZR
	mov	d16, XZR
	mov	d17, XZR
	mov	d18, XZR
	mov	d19, XZR
	mov	d20, XZR
	mov	d21, XZR
	mov	d22, XZR
	mov	d23, XZR
	mov	d24, XZR
	mov	d25, XZR
	mov	d26, XZR
	mov	d27, XZR
	mov	d28, XZR
	mov	d29, XZR
	mov	d30, XZR
	mov	d31, XZR
#endif /* BSP_START_NEEDS_REGISTER_INITIALIZATION */

#endif /* AARCH64_MULTILIB_VFP */

	/*
	 * Invoke the start hook 0.
	 *
	 */

	mov	x1, x5		/* machine type number or ~0 for DT boot */
	bl	bsp_start_hook_0

	/* Branch to start hook 1 */
	bl	bsp_start_hook_1

	/* Branch to boot card */
	mov	x0, #0
	bl	boot_card
