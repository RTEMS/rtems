/*
 * Atmel AT91RM9200 Interrupt handler
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopgindog.com>
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
        
        .globl bsp_interrupt_dispatch
bsp_interrupt_dispatch :
/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load vector into r0 and handler address into r1.
 */
        ldr     r0, =0xFFFFF100   /* AIC_CTL_BASE + AIC_IVR */
        ldr     r1, [r0]                
        str     r1, [r0]          /* write back in case we are using protect */

	ldr     r0, =0xFFFFF108   /* AIC_CTL_BASE + AIC_ISR */
        ldr     r0, [r0]          /* Read interrupt vector  */

        stmdb   sp!,{lr}
        ldr     lr, =IRQ_return   /* prepare the return from handler  */

        mov     pc, r1            /* execute handler */

IRQ_return:
        ldr   r2, =0xFFFFF130     /* AIC_CTL_BASE + AIC_EIOCR */
        str   r1, [r2]
       
        ldmia sp!,{lr}

        mov pc, lr
