/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM exception support implementation.
 */

/*
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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Moved from file 'cpukit/score/cpu/arm/cpu_asm.S'.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

        .text

/* FIXME:       _Exception_Handler_Undef_Swi is untested */
DEFINE_FUNCTION_ARM(_Exception_Handler_Undef_Swi)
/* FIXME: This should use load and store multiple instructions */
	sub     r13,r13,#SIZE_REGS
	str     r4,  [r13, #REG_R4]
	str     r5,  [r13, #REG_R5]
	str     r6,  [r13, #REG_R6]
	str     r7,  [r13, #REG_R7]
	str     r8,  [r13, #REG_R8]
	str     r9,  [r13, #REG_R9]
	str     r10, [r13, #REG_R10]
	str     r11, [r13, #REG_R11]
	str     sp,  [r13, #REG_SP]
	str     lr,  [r13, #REG_LR]
	mrs	r0,  cpsr		/* read the status */
	and	r0,  r0,#0x1f		/* we keep the mode as exception number */
	str	r0,  [r13, #REG_PC]     /* we store it in a free place */
	mov	r0,  r13		/* put frame address in r0 (C arg 1) */

	ldr	r1, =SWI_Handler
	ldr     lr, =_go_back_1
	ldr	pc,[r1]				/* call handler  */
_go_back_1:
	ldr     r4,  [r13, #REG_R4]
	ldr     r5,  [r13, #REG_R5]
	ldr     r6,  [r13, #REG_R6]
	ldr     r7,  [r13, #REG_R7]
	ldr     r8,  [r13, #REG_R8]
	ldr     r9,  [r13, #REG_R9]
	ldr     r10, [r13, #REG_R10]
	ldr     r11, [r13, #REG_R11]
	ldr     sp,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
	movs	pc,r14			/* return  */
	
/* FIXME:       _Exception_Handler_Abort is untested */
DEFINE_FUNCTION_ARM(_Exception_Handler_Abort)
/* FIXME: This should use load and store multiple instructions */
	sub     r13,r13,#SIZE_REGS
	str     r4,  [r13, #REG_R4]
	str     r5,  [r13, #REG_R5]
	str     r6,  [r13, #REG_R6]
	str     r7,  [r13, #REG_R7]
	str     r8,  [r13, #REG_R8]
	str     r9,  [r13, #REG_R9]
	str     sp,  [r13, #REG_R11]
	str     lr,  [r13, #REG_SP]
	str     lr,  [r13, #REG_LR]
	mrs	r0,  cpsr		/* read the status */
	and	r0,  r0,#0x1f		/* we keep the mode as exception number */
	str	r0,  [r13, #REG_PC]     /* we store it in a free place */
	mov	r0,  r13		/* put frame address in ro (C arg 1) */
	
	ldr	r1, =_currentExcHandler
	ldr     lr, =_go_back_2
	ldr	pc,[r1]				/* call handler  */
_go_back_2:
	ldr     r4,  [r13, #REG_R4]
	ldr     r5,  [r13, #REG_R5]
	ldr     r6,  [r13, #REG_R6]
	ldr     r7,  [r13, #REG_R7]
	ldr     r8,  [r13, #REG_R8]
	ldr     r9,  [r13, #REG_R9]
	ldr     r10, [r13, #REG_R10]
	ldr     sp,  [r13, #REG_R11]
	ldr     lr,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
#ifdef  __thumb__
	subs	r11, r14,#4
	bx	r11
	nop
#else
	subs	pc,r14,#4			/* return */
#endif

#define ABORT_REGS_OFFS 32-REG_R4
#define ABORT_SIZE_REGS SIZE_REGS+ABORT_REGS_OFFS
	
DEFINE_FUNCTION_ARM(_exc_data_abort)
	sub	sp, sp, #ABORT_SIZE_REGS	/* reserve register frame */
	stmia	sp, {r0-r11}
	add	sp, sp, #ABORT_REGS_OFFS	/* the Context_Control structure starts by CPSR, R4, ... */

	str	ip, [sp, #REG_PC]		/* store R12 (ip) somewhere, oh hackery, hackery, hack */
	str	lr, [sp, #REG_LR]

	mov	r1, lr
	ldr	r0, [r1, #-8]			/* r0 = bad instruction */
	mrs	r1, spsr			/* r1 = spsr */
	mov	r2, r13				/* r2 = exception frame of Context_Control type */
#if defined(__thumb__)
	.code 32
	/*arm to thumb*/
	adr	r5, to_thumb + 1
	bx	r5
	.code 16
to_thumb:	
#endif	
	bl	do_data_abort
#if defined(__thumb__)
/*back to arm*/		
	.code 16
thumb_to_arm:
	.align 2
	adr r5, arm_code
	bx	r5
	nop
	.code 32
arm_code:
#endif
	
	ldr	lr, [sp, #REG_LR]
	ldr	ip, [sp, #REG_PC]		/* restore R12 (ip) */

	sub	sp, sp, #ABORT_REGS_OFFS
	ldmia	sp, {r0-r11}
	add	sp, sp, #ABORT_SIZE_REGS
#ifdef  __thumb__
	subs	r11, r14, #4			/* return to the instruction */
	bx      r11
	nop
#else
	subs	pc, r14, #4
#endif
						/* _AFTER_ the aborted one */

#endif /* ARM_MULTILIB_ARCH_V4 */
