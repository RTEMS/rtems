/*  cpu_asm.s
 *
 *  This file contains all assembly code for the ARM implementation
 *  of RTEMS.
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

/*
 * Format of ARM Register structure
 */

.set REG_R0,  0
.set REG_R1,  4
.set REG_R2,  8
.set REG_R3,  12
.set REG_R4,  16
.set REG_R5,  20
.set REG_R6,  24
.set REG_R7,  28
.set REG_R8,  32
.set REG_R9,  36
.set REG_R10, 40
.set REG_FP,  44
.set REG_IP,  48
.set REG_SP,  52
.set REG_LR,  56
.set REG_PC,  60
.set SIZE_REGS,   REG_PC + 4

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 *
 * R0 = run_context    R1 = heir_context
 *	
 */

        .globl _CPU_Context_switch

_CPU_Context_switch:
	str     r2,  [r0, #REG_R2]
	str     r3,  [r0, #REG_R3]
	str     r4,  [r0, #REG_R4]
	str     r5,  [r0, #REG_R5]
	str     r6,  [r0, #REG_R6]
	str     r7,  [r0, #REG_R7]
	str     r8,  [r0, #REG_R8]
	str     r9,  [r0, #REG_R9]
	str     r10, [r0, #REG_R10]
	str     sp,  [r0, #REG_SP]
	str     lr,  [r0, #REG_PC]
	
	ldr     r2,  [r1, #REG_R2]
	ldr     r3,  [r1, #REG_R3]
	ldr     r4,  [r1, #REG_R4]
	ldr     r5,  [r1, #REG_R5]
	ldr     r6,  [r1, #REG_R6]
	ldr     r7,  [r1, #REG_R7]
	ldr     r8,  [r1, #REG_R8]
	ldr     r9,  [r1, #REG_R9]
	ldr     r10, [r1, #REG_R10]
	ldr     sp,  [r1, #REG_SP]
	ldr     lr,  [r1, #REG_PC]
	mov	pc, lr

/*
 *  NOTE: May be unnecessary to reload some registers.
 */

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context.
 */
        .globl _CPU_Context_restore

_CPU_Context_restore:

	ldr     r2,  [r0, #REG_R2]
	ldr     r3,  [r0, #REG_R3]
	ldr     r4,  [r0, #REG_R4]
	ldr     r5,  [r0, #REG_R5]
	ldr     r6,  [r0, #REG_R6]
	ldr     r7,  [r0, #REG_R7]
	ldr     r8,  [r0, #REG_R8]
	ldr     r9,  [r0, #REG_R9]
	ldr     r10, [r0, #REG_R10]
	ldr     sp,  [r0, #REG_SP]
	ldr     lr,  [r0, #REG_PC]
	mov     pc,  lr	


        .globl _Exception_Handler_Undef_Swi
_Exception_Handler_Undef_Swi:
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
	str     fp,  [r13, #REG_FP]
	str     ip,  [r13, #REG_IP]
	str     sp,  [r13, #REG_SP]
	str     lr,  [r13, #REG_LR]
	mrs	r0,  cpsr		/* read the status */
	and	r0,  r0,#0x1f		/* we keep the mode as exception number */
	str	r0,  [r13, #REG_PC]     /* we store it in a free place */
	mov	r0,  r13		/* put frame address in r0 (C arg 1) */

	ldr	r1, =_currentExcHandler
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
	ldr     fp,  [r13, #REG_FP]
	ldr     ip,  [r13, #REG_IP]
	ldr     sp,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
	movs	pc,r14				/* return  */

        .globl _Exception_Handler_Abort
_Exception_Handler_Abort:
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
	str     sp,  [r13, #REG_FP]
	str     lr,  [r13, #REG_IP]
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
	ldr     sp,  [r13, #REG_FP]
	ldr     lr,  [r13, #REG_IP]
	ldr     lr,  [r13, #REG_SP]
	ldr     lr,  [r13, #REG_LR]
	add     r13,r13,#SIZE_REGS
	subs	pc,r14,#4			/* return */
	

