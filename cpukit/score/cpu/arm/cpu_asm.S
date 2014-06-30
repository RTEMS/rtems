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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

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

#ifdef ARM_MULTILIB_VFP_D32
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

	/* Wait for heir context to stop execution */
1:
	ldrb	r3, [r1, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]
	cmp	r3, #0
	bne	1b

	/* The heir context executes now on this processor */
	dmb
	mov	r3, #1
	strb	r3, [r1, #ARM_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]
#endif

/* Start restoring context */
.L_restore:
#ifdef ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE
	clrex
#endif

#ifdef ARM_MULTILIB_HAS_THREAD_ID_REGISTER
	ldr	r3, [r1, #ARM_CONTEXT_CONTROL_THREAD_ID_OFFSET]
	mcr	p15, 0, r3, c13, c0, 3
#endif

#ifdef ARM_MULTILIB_VFP_D32
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

#endif /* ARM_MULTILIB_ARCH_V4 */
