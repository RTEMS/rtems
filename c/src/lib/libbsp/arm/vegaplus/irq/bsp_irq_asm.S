/* bsp_irq_asm.S
 *
 *  This file contains the implementation of the IRQ handler
 *  for a specific BSP
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#define __asm__
#include  <registers.h>
		
/* 
 * Function to obtain, execute an IT handler and acknowledge the IT 
 */

	.globl ExecuteITHandler
	
ExecuteITHandler :	
  ldr	r0, =INTPHAI3            /* read the vector number */
  ldr	r0, [r0]	
  ldr	r0, [r0]		/* extract the IT handler @ */

  /*
   * re-enable interrupts at processor level as the current
   * interrupt source is now masked via VEGA logic
   */
  mrs	r1, cpsr
  and	r1, r1, #0xFFFFFF3F
  msr	cpsr, r1

  stmdb	  sp!,{lr}
  ldr     lr, =IRQ_return         /* prepare the return from handler  */
  mov     pc, r0			/* EXECUTE INT HANDLER */

IRQ_return:
  ldmia sp!,{lr}
  ldr	r2, =INTSTAT3		/* Ack the interrupt */
  ldr	r2, [r2]
  ldr	r1, =INTACK3
  str	r2, [r1]
  /*
   * disable interrupts_again and switch back to SVC mode
   */
  mrs	r0, cpsr
  orr	r0, r0, #0xc3
  msr	cpsr, r0

  mov pc, lr
	
/* 
 * Function to acknowledge the IT controller 
 */
	.globl AckControler

#if 0	
AckControler:	  
  ldr	r0, =INTEOI3
  mov   r1, #EOI
  str	r1, [r0]

  b	ReturnFromAck		/* return to ISR handler */
#endif
