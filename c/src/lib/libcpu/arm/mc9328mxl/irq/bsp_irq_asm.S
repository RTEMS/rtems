/*
 * MC9328MXL Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  bsp_irq_asm.S,v 1.1 2002/11/13 17:55:06 joel Exp
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
  ldr 	r0, =0x00223040  /* Read the vector number */
  ldr	r1, [r0]
  mov   r1, r1, LSR #16         /* get the NIVECTOR into 16 LSbits */

  /* find the ISR's address based on the vector */	
  ldr   r0, =bsp_vector_table
  ldr	r0, [r0, r1, LSL #2]	/* Read the address */


  stmdb	  sp!,{lr}
  ldr     lr, =IRQ_return         /* prepare the return from handler  */

  mov     pc, r0		/* EXECUTE INT HANDLER */

IRQ_return:
  ldmia sp!,{lr}

  mov pc, lr
