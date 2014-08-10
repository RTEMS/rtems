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
 *  Copyright (c) 2013-2014 embedded brains GmbH
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
	mrs	r2, CPSR
	stmia	r0,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}

#ifdef ARM_MULTILIB_VFP
	add	r3, r0, #ARM_CONTEXT_CONTROL_D8_OFFSET
	vstm	r3, {d8-d15}
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	mrc	p15, 0, r3, c13, c0, 3
	str	r3, [r0, #ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET]
#endif

#ifdef RTEMS_SMP
	/* The executing context no longer executes on this processor */
	dmb
	mov	r3, #0
	strb	r3, [r0, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]

.L_check_is_executing:

	/* Check the is executing indicator of the heir context */
	add	r3, r1, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET
	ldrexb	r4, [r3]
	cmp	r4, #0
	bne	.L_check_thread_dispatch_necessary

	/* Try to update the is executing indicator of the heir context */
	mov	r4, #1
	strexb	r5, r4, [r3]
	cmp	r5, #0
	bne	.L_check_thread_dispatch_necessary
	dmb
#endif

/* Start restoring context */
.L_restore:
#if !defined(RTEMS_SMP) && defined(ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE)
	clrex
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	ldr	r3, [r1, #ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET]
	mcr	p15, 0, r3, c13, c0, 3
#endif

#ifdef ARM_MULTILIB_VFP
	add	r3, r1, #ARM_CONTEXT_CONTROL_D8_OFFSET
	vldm	r3, {d8-d15}
#endif

	ldmia	r1,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}
	msr	CPSR_fsxc, r2
#ifdef __thumb__
	bx	lr
	nop
#else
	mov	pc, lr
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
        b       .L_restore

#ifdef RTEMS_SMP
.L_check_thread_dispatch_necessary:

	GET_SELF_CPU_CONTROL	r2, r3

	/* Check if a thread dispatch is necessary */
	ldrb	r4, [r2, #PER_CPU_DISPATCH_NEEDED]
	cmp	r4, #0
	beq	.L_check_is_executing

	/* We have a new heir */

	/* Clear the thread dispatch necessary flag */
	mov	r4, #0
	strb	r4, [r2, #PER_CPU_DISPATCH_NEEDED]
	dmb

	/* Read the executing and heir */
	ldr	r4, [r2, #PER_CPU_OFFSET_EXECUTING]
	ldr	r5, [r2, #PER_CPU_OFFSET_HEIR]

	/* Calculate the heir context pointer */
	sub	r4, r1, r4
	add	r1, r5, r4

	/* Update the executing */
	str	r5, [r2, #PER_CPU_OFFSET_EXECUTING]

	b	.L_check_is_executing
#endif

#endif /* ARM_MULTILIB_ARCH_V4 */
