/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM architecture support implementation.
 */

/*
 *  This file contains all assembly code for the ARM implementation
 *  of RTEMS.
 *
 *  Copyright (c) 2007 by Ray Xu, <Rayx.cn@gmail.com>
 *          Thumb support added.
 *
 *  Copyright (c) 2002 by Advent Networks, Inc.
 *          Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2013, 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

        .text

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *  void _CPU_Context_restore( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 *
 *  R0 = run_context    R1 = heir_context
 *
 *  This function copies the current registers to where r0 points, then
 *  restores the ones from where r1 points.
 *
 *  Using the ldm/stm opcodes save 2-3 us on 100 MHz ARM9TDMI with
 *  a 16 bit data bus.
 *
 */

DEFINE_FUNCTION_ARM(_CPU_Context_switch)
/* Start saving context */
	GET_SELF_CPU_CONTROL	r2
	stm	r0, {r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	mrc	p15, 0, r3, c13, c0, 3
#endif

	ldr	r4, [r2, #PER_CPU_ISR_DISPATCH_DISABLE]

#ifdef ARM_MULTILIB_VFP
	add	r5, r0, #ARM_CONTEXT_CONTROL_D8_OFFSET
	vstm	r5, {d8-d15}
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	str	r3, [r0, #ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET]
#endif

	str	r4, [r0, #ARM_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE]

#ifdef RTEMS_SMP
	/*
	 * The executing thread no longer executes on this processor.  Switch
	 * the stack to the temporary interrupt stack of this processor.  Mark
	 * the context of the executing thread as not executing.
	 */
	dmb
	add	sp, r2, #(PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE)
	mov	r3, #0
	strb	r3, [r0, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]

.L_check_is_executing:

	/* Check the is executing indicator of the heir context */
	add	r3, r1, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET
	ldrexb	r4, [r3]
	cmp	r4, #0
	bne	.L_get_potential_new_heir

	/* Try to update the is executing indicator of the heir context */
	mov	r4, #1
	strexb	r5, r4, [r3]
	cmp	r5, #0
	bne	.L_get_potential_new_heir
	dmb
#endif

/* Start restoring context */
.L_restore:
#if !defined(RTEMS_SMP) && defined(ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE)
	clrex
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	ldr	r3, [r1, #ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET]
#endif

	ldr	r4, [r1, #ARM_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE]

#ifdef ARM_MULTILIB_VFP
	add	r5, r1, #ARM_CONTEXT_CONTROL_D8_OFFSET
	vldm	r5, {d8-d15}
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	mcr	p15, 0, r3, c13, c0, 3
#endif

	str	r4, [r2, #PER_CPU_ISR_DISPATCH_DISABLE]

	/* In ARMv5T and above the load of PC is an interworking branch */
#if __ARM_ARCH >= 5
	ldm	r1, {r4, r5, r6, r7, r8, r9, r10, r11, r13, pc}
#else
	ldm	r1, {r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}
	bx	lr
#endif

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This function copies the restores the registers from where r0 points.
 *  It must match _CPU_Context_switch()
 *
 */
DEFINE_FUNCTION_ARM(_CPU_Context_restore)
        mov     r1, r0
	GET_SELF_CPU_CONTROL	r2
        b       .L_restore

#ifdef RTEMS_SMP
.L_get_potential_new_heir:

	/* We may have a new heir */

	/* Read the executing and heir */
	ldr	r4, [r2, #PER_CPU_OFFSET_EXECUTING]
	ldr	r5, [r2, #PER_CPU_OFFSET_HEIR]

	/*
	 * Update the executing only if necessary to avoid cache line
	 * monopolization.
	 */
	cmp	r4, r5
	beq	.L_check_is_executing

	/* Calculate the heir context pointer */
	sub	r4, r1, r4
	add	r1, r5, r4

	/* Update the executing */
	str	r5, [r2, #PER_CPU_OFFSET_EXECUTING]

	b	.L_check_is_executing
#endif

#endif /* ARM_MULTILIB_ARCH_V4 */
