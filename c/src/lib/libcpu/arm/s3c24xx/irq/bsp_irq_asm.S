/* bsp_irq_asm.S
 *
 *  This file contains the implementation of the IRQ handler
 *  for a specific BSP
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 * merged to common file for s32400 and s32410 by Thomas Doerfler, embedded brains
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <s3c24xx.h>

/*
 * Function to obtain, execute an IT handler and acknowledge the IT
 */

        .globl bsp_interrupt_dispatch

bsp_interrupt_dispatch :

        ldr     r0, =rINTOFFSET_ADDR /* Read rINTOFFSET */
        ldr     r1, [r0]

        ldr     r0, =bsp_vector_table
        ldr     r0, [r0, r1, LSL #2]    /* Read the address */

        stmdb     sp!,{lr}
        ldr     lr, =IRQ_return         /* prepare the return from handler  */

        mov pc, r0

IRQ_return:
        ldmia sp!,{lr}

        mov pc, lr
