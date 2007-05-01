/*
 *  LPC22XX/LPC21xx Intererrupt handler
 *
 *  Modified by Ray <rayx.cn@gmail.com> 2006 from Jay Monkman's code
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
 */
#define __asm__
	
/* 
 * BSP specific interrupt handler for INT or FIQ. In here
 * you do determine which interrupt happened and call its
 * handler.
 */
	.globl ExecuteITHandler
ExecuteITHandler :

/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load handler address into r0.
 */
 
  ldr 	r0, =0xFFFFF030  /* Read the vector number */
  ldr	r1, [r0]

  /* find the ISR's address based on the vector VICVectAddr0 */	
  /*ldr   r0, =0xFFFFF100*/
  /*ldr	r0, [r0, r1, LSL #2]*/	/* Read the address */


  stmdb	  sp!,{lr}
  ldr     lr, =IRQ_return         /* prepare the return from handler  */

  mov     pc, r1		/* EXECUTE INT HANDLER */

IRQ_return:
  ldmia sp!,{lr}

  mov pc, lr
