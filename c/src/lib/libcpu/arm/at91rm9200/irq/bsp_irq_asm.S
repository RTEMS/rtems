/*
 * Atmel AT91RM9200 Interrupt handler
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopgindog.com>
 *      
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
 */
#define __asm__
        
        .globl ExecuteITHandler
ExecuteITHandler :
/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load handler address into r0.
 */
        ldr     r0, =0xFFFFF100   /* AIC_CTL_BASE + AIC_IVR */
        ldr     r1, [r0]                
        str     r1, [r0]          /* write back in case we are using protect */

        stmdb   sp!,{lr}
        ldr     lr, =IRQ_return   /* prepare the return from handler  */

        mov     pc, r1            /* execute handler */

IRQ_return:
        ldr   r2, =0xFFFFF130     /* AIC_CTL_BASE + AIC_EIOCR */
        str   r1, [r2]
       
        ldmia sp!,{lr}

        mov pc, lr
