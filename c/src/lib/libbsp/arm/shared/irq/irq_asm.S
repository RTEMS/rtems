/* irq_asm.S
 *
 *  This file contains the implementation of the IRQ handler
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "asm.h"
#define __asm__
#include <registers.h>


/*
 * WARNING :	register r5 is important. If you need to use it,
 *	        to forget to save it !!!!!!!!!!
 */
	
	.globl _ISR_Handler
_ISR_Handler:	
	stmdb   sp!, {r4,r5,lr}			/* save regs on INT stack */
	mrs	r4, cpsr			/* save current CSPR */
	mov	r5, r4                          /* copy CSPR */
	orr	r4, r4, #3
	msr	cpsr, r4			/* switch to SVC mode */
	
	stmdb   sp!, {r0-r3,r12,r14}	        /* save scratch regs on SVC stack */

	msr	cpsr, r5			/* switch back to INT mode */

        ldr	r0, =_ISR_Nest_level             /* one nest level deeper */
	ldr     r1, [r0]
	add	r1, r1,#1
	str     r1, [r0]
	
        ldr	r0, =_Thread_Dispatch_disable_level          /* disable multitasking */
	ldr     r1, [r0]
	add	r1, r1,#1
	str     r1, [r0]

	b	ExecuteITHandler	        /* BSP specific function to INT handler */

	.globl	ReturnFromHandler
ReturnFromHandler :	
        ldr	r0, =_ISR_Nest_level			/* one less nest level  */
	ldr     r1, [r0]
	sub	r1, r1,#1
	str     r1, [r0]
	
        ldr	r0, =_Thread_Dispatch_disable_level	/* unnest multitasking */
	ldr     r1, [r0]
	sub	r1, r1,#1
	str     r1, [r0]

	cmp	r1, #0	                 /* is dispatch enabled */
        bne     exitit                     /* Yes, then exit */

        ldr	r0, =_Context_Switch_necessary	/* task switch necessary ? */
	ldr     r1, [r0]
	cmp	r1, #0
	bne     schedule			/* yes, call scheduler */
	
        ldr	r0, =_ISR_Signals_to_thread_executing	
	ldr     r1, [r0]		/* signals sent to Run_thread */
	cmp	r1, #0			/*     while in interrupt handler ? */
	beq     exitit			/* No, exit */
	
bframe:
        mov	r1, #0       /* _ISR_Signals_to_thread_executing = FALSE */
	str	r1, [r0]
	/*
	 * At this point, we need a complete exception context for the
	 * current thread. We need to complete the interrupt exception
	 * with the "not-yet-saved" registers
	 */
	/* 
	 * currently exception context = interrupt handler
	 * it needs to be optimized 
	 */
	bl	_ThreadProcessSignalsFromIrq
	b	exitit
				
schedule:
	/*
	 * the scratch registers have already been saved and we are already
	 * back on the thread system stack. So we can call _Thread_Displatch 
	 * directly
	 */
	bl	_Thread_Dispatch
	/*
	 * fall through exit to restore complete contex (scratch registers
	 * eip, CS, Flags).
	 */
exitit:
	b	AckControler		/* BSP specific function to ack PIC */

	.globl ReturnFromAck
ReturnFromAck :	
	ldmia   sp!, {r0-r3,r12,r14}	/* restore regs from SVC stack */
	msr	cpsr, r5		/* switch back to INT mode */
	ldmia	sp!, {r4,r5,lr}		/* restore regs from INT stack */		
	subs	pc,r14,#4		/* return */
	
