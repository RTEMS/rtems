/*
 *  $Id$
 *
 *  This file contains all assembly code for the ARM implementation
 *  of RTEMS.
 *
 *  Copyright (c) 2002 by Advent Networks, Inc.
 *          Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

#include <asm.h>
#include <rtems/score/cpu_asm.h>


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
        .globl _CPU_Context_switch
_CPU_Context_switch:
/* Start saving context */
	mrs	r2, cpsr
	stmia	r0,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}


/* Start restoring context */
_restore:       
	ldmia	r1,  {r2, r4, r5, r6, r7, r8, r9, r10, r11, r13, r14}
	msr	cpsr, r2
	mov	pc, lr

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This function copies the restores the registers from where r0 points.
 *  It must match _CPU_Context_switch()
 *
 */
        .globl _CPU_Context_restore
_CPU_Context_restore:
        mov     r1, r0
        b       _restore



/* FIXME:       _Exception_Handler_Undef_Swi is untested */
        .globl _Exception_Handler_Undef_Swi
_Exception_Handler_Undef_Swi:
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
        .globl _Exception_Handler_Abort
_Exception_Handler_Abort:
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
	subs	pc,r14,#4			/* return */
	
	.globl _exc_data_abort
_exc_data_abort:
	sub	sp, sp, #SIZE_REGS		/* reserve register frame */
	stmia	sp, {r0-r12}
	str	lr, [sp, #REG_LR]
	mov	r1, lr
	ldr	r0, [r1, #-8]			/* r0 = bad instruction */
	mrs	r1, spsr			/* r1 = spsr */
	mov	r2, r13				/* r2 = exception frame */
	bl	do_data_abort
	
	ldr	lr, [sp, #REG_LR]
	ldmia	sp, {r0-r12}
	add	sp, sp, #SIZE_REGS

	subs	pc, r14, #4			/* return to the instruction */
						/* _AFTER_ the aborted one */
