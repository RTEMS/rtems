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
 *
 *  NOTE:  The function should be able to only save/restore the registers
 *         that would be saved by a C function since the others have already
 *         been saved.
 *        
 *         It should also be able to use the stm/ldm instructions.
 *        
 */

        .globl _CPU_Context_switch
_CPU_Context_switch:
/* FIXME: This should use load and store multiple instructions */
/* Start saving context */
	str     r2,  [r0, #REG_R2]
	str     r3,  [r0, #REG_R3]
	str     r4,  [r0, #REG_R4]
	str     r5,  [r0, #REG_R5]
	str     r6,  [r0, #REG_R6]
	str     r7,  [r0, #REG_R7]
	str     r8,  [r0, #REG_R8]
	str     r9,  [r0, #REG_R9]
	str     r10, [r0, #REG_R10]
	
	str     r11, [r0, #REG_R11]
	str     r12, [r0, #REG_R12]	
	
	str     sp,  [r0, #REG_SP]
	str     lr,  [r0, #REG_PC]     /* save LR at PC's location */
	
	mrs	r2, cpsr
	str	r2,  [r0, #REG_CPSR]

/* Start restoring context */
        
       	ldr	r2,  [r1, #REG_CPSR]
	msr	cpsr, r2

	ldr     r2,  [r1, #REG_R2]
	ldr     r3,  [r1, #REG_R3]
	ldr     r4,  [r1, #REG_R4]
	ldr     r5,  [r1, #REG_R5]
	ldr     r6,  [r1, #REG_R6]
	ldr     r7,  [r1, #REG_R7]
	ldr     r8,  [r1, #REG_R8]
	ldr     r9,  [r1, #REG_R9]
	ldr     r10, [r1, #REG_R10]	
	ldr     r11, [r1, #REG_R11]
	ldr     r12, [r1, #REG_R12]	
	
	ldr     sp,  [r1, #REG_SP]
	ldr     lr,  [r1, #REG_PC]	
	mov	pc, lr

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This function copies the restores the registers from where r0 points.
 *  It must match _CPU_Context_switch()
 *
 *  NOTE:  The function should be able to only save/restore the registers
 *         that would be saved by a C function since the others have already
 *         been saved.
 *        
 *         It should also be able to use the stm/ldm instructions.
 *        
 */
        .globl _CPU_Context_restore
_CPU_Context_restore:
/* FIXME: This should use load and store multiple instructions */
	ldr	r2,  [r0, #REG_CPSR]
	msr	cpsr, r2

	ldr     r2,  [r0, #REG_R2]	
	ldr     r3,  [r0, #REG_R3]
	ldr     r4,  [r0, #REG_R4]
	ldr     r5,  [r0, #REG_R5]
	ldr     r6,  [r0, #REG_R6]
	ldr     r7,  [r0, #REG_R7]
	ldr     r8,  [r0, #REG_R8]
	ldr     r9,  [r0, #REG_R9]
	ldr     r10, [r0, #REG_R10]	
	ldr     r11, [r1, #REG_R11]
	ldr     r12, [r1, #REG_R12]	
			
	ldr     sp,  [r0, #REG_SP]
	ldr     lr,  [r0, #REG_PC]
	mov     pc,  lr	


/* FIXME:       _Exception_Handler_Undef_Swi is untested */
        .globl _Exception_Handler_Undef_Swi
_Exception_Handler_Undef_Swi:
/* FIXME: This should use load and store multiple instructions */
	sub     r13,r13,#SIZE_REGS
	str     r0,  [r13, #REG_R0]
	str     r1,  [r13, #REG_R1]
	str     r2,  [r13, #REG_R2]
	str     r3,  [r13, #REG_R3]
	str     r4,  [r13, #REG_R4]
	str     r5,  [r13, #REG_R5]
	str     r6,  [r13, #REG_R6]
	str     r7,  [r13, #REG_R7]
	str     r8,  [r13, #REG_R8]
	str     r9,  [r13, #REG_R9]
	str     r10, [r13, #REG_R10]
	str     r11, [r13, #REG_R11]
	str     r12, [r13, #REG_R12]
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
	ldr     r0,  [r13, #REG_R0]
	ldr     r1,  [r13, #REG_R1]
	ldr     r2,  [r13, #REG_R2]
	ldr     r3,  [r13, #REG_R3]
	ldr     r4,  [r13, #REG_R4]
	ldr     r5,  [r13, #REG_R5]
	ldr     r6,  [r13, #REG_R6]
	ldr     r7,  [r13, #REG_R7]
	ldr     r8,  [r13, #REG_R8]
	ldr     r9,  [r13, #REG_R9]
	ldr     r10, [r13, #REG_R10]
	ldr     r11, [r13, #REG_R11]
	ldr     r12, [r13, #REG_R12]
	ldr     sp,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
	movs	pc,r14			/* return  */ 
	
/* FIXME:       _Exception_Handler_Abort is untested */
        .globl _Exception_Handler_Abort
_Exception_Handler_Abort:
/* FIXME: This should use load and store multiple instructions */
	sub     r13,r13,#SIZE_REGS
	str     r0,  [r13, #REG_R0]
	str     r1,  [r13, #REG_R1]
	str     r2,  [r13, #REG_R2]
	str     r3,  [r13, #REG_R3]
	str     r4,  [r13, #REG_R4]
	str     r5,  [r13, #REG_R5]
	str     r6,  [r13, #REG_R6]
	str     r7,  [r13, #REG_R7]
	str     r8,  [r13, #REG_R8]
	str     r9,  [r13, #REG_R9]
	str     r10, [r13, #REG_R10]
	str     sp,  [r13, #REG_R11]
	str     lr,  [r13, #REG_R12]
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
	ldr     r0,  [r13, #REG_R0]
	ldr     r1,  [r13, #REG_R1]
	ldr     r2,  [r13, #REG_R2]
	ldr     r3,  [r13, #REG_R3]
	ldr     r4,  [r13, #REG_R4]
	ldr     r5,  [r13, #REG_R5]
	ldr     r6,  [r13, #REG_R6]
	ldr     r7,  [r13, #REG_R7]
	ldr     r8,  [r13, #REG_R8]
	ldr     r9,  [r13, #REG_R9]
	ldr     r10, [r13, #REG_R10]
	ldr     sp,  [r13, #REG_R11]
	ldr     lr,  [r13, #REG_R12]
	ldr     lr,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
	subs	pc,r14,#4			/* return */
	
