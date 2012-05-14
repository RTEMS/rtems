/*
 *  This file contains all assembly code for the
 *  LM32 implementation of RTEMS.
 *
 *  Derived from no_cpu/cpu_asm.S, copyright (c) 1989-1999,
 *    On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 *
 *  Michael Walle <michael@walle.cc>, 2009
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>

/*  void _CPU_Context_switch(run_context, heir_context)
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  LM32 Specific Information:
 *
 *  Saves/restores all callee-saved general purpose registers as well as
 *  the stack pointer, return address and interrupt enable status register
 *  to/from the context.
 *
 */
    .globl _CPU_Context_switch
_CPU_Context_switch:
    sw      (r1+0), r11   /* r1 is the first argument */
    sw      (r1+4), r12
    sw      (r1+8), r13
    sw      (r1+12), r14
    sw      (r1+16), r15
    sw      (r1+20), r16
    sw      (r1+24), r17
    sw      (r1+28), r18
    sw      (r1+32), r19
    sw      (r1+36), r20
    sw      (r1+40), r21
    sw      (r1+44), r22
    sw      (r1+48), r23
    sw      (r1+52), r24
    sw      (r1+56), r25
    sw      (r1+60), gp
    sw      (r1+64), fp
    sw      (r1+68), sp
    sw      (r1+72), ra
    rcsr    r3, IE
    sw      (r1+76), r3
    .extern _exception_stack_frame
    mvhi    r3, hi(_exception_stack_frame)
    ori     r3, r3, lo(_exception_stack_frame)
    lw      r4, (r3+0)
    be      r4, r0, 2f
1:
    lw      r5, (r4+44)
    sw      (r3+0), r0
    bi      3f
2:
    mvhi    r5, hi(_Thread_Dispatch)
    ori     r5, r5, lo(_Thread_Dispatch)
3:
    sw      (r1+80), r5

_CPU_Context_switch_restore:
    lw      r11, (r2+0)   /* r2 is the second argument */
    lw      r12, (r2+4)
    lw      r13, (r2+8)
    lw      r14, (r2+12)
    lw      r15, (r2+16)
    lw      r16, (r2+20)
    lw      r17, (r2+24)
    lw      r18, (r2+28)
    lw      r19, (r2+32)
    lw      r20, (r2+36)
    lw      r21, (r2+40)
    lw      r22, (r2+44)
    lw      r23, (r2+48)
    lw      r24, (r2+52)
    lw      r25, (r2+56)
    lw      gp, (r2+60)
    lw      fp, (r2+64)
    lw      sp, (r2+68)
    lw      ra, (r2+72)
    lw      r3, (r2+76)
    wcsr    IE, r3
    ret

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  LM32 Specific Information:
 *
 *  Moves argument #1 to #2 and branches to the restore part of the
 *  context switch code above.
 */
    .globl _CPU_Context_restore
_CPU_Context_restore:
    mv      r2, r1
    bi      _CPU_Context_switch_restore

/*  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  LM32 Specific Information:
 *
 *  Saves all the caller-saved general purpose registers as well as the
 *  return address, exception return address and breakpoint return address
 *  (the latter may be unnecessary) onto the stack, which is either the task
 *  stack (in case of a interrupted task) or the interrupt stack (if an
 *  interrupt was interrupted).
 *  After that, it figures out the pending interrupt with the highest
 *  priority and calls the main ISR handler written in C, which in turn
 *  handles interrupt nesting, software interrupt stack setup etc and
 *  finally calls the user ISR.
 *  At the end the saved registers are restored.
 *
 */

    .globl  _ISR_Handler
_ISR_Handler:
    xor      r0, r0, r0
    addi     sp, sp, -52
    sw       (sp+4), r1
    sw       (sp+8), r2
    sw       (sp+12), r3
    sw       (sp+16), r4
    sw       (sp+20), r5
    sw       (sp+24), r6
    sw       (sp+28), r7
    sw       (sp+32), r8
    sw       (sp+36), r9
    sw       (sp+40), r10
    sw       (sp+44), ra
    sw       (sp+48), ea
    sw       (sp+52), ba

    /*
     * Scan through IP & IM bits starting from LSB until irq vector is
     * found. The vector is stored in r1, which is the first argument for
     * __ISR_Handler.
     */
    rcsr    r2, IP
    rcsr    r3, IM
    mv      r1, r0        /* r1: counter for the vector number */
    and     r2, r2, r3    /* r2: pending irqs, which are enabled */
    mvi     r3, 1         /* r3: register for the walking 1 */
    /*
     * If r2 is zero, there was no interrupt.
     * This should never happen!
     */
    be      r2, r0, exit_isr
find_irq:
    and     r4, r2, r3
    bne     r4, r0, found_irq
    sli     r3, r3, 1
    addi    r1, r1, 1
    bi      find_irq

found_irq:
    /*
     * Call __ISR_Handler for further processing.
     * r1 is the vector number, calculated above
     * r2 is the pointer to the CPU_Interrupt_frame
     */
    addi    r2, sp, 4

    .extern __ISR_Handler
    mvhi    r3, hi(__ISR_Handler)
    ori     r3, r3, lo(__ISR_Handler)
    call    r3

exit_isr:
    /* Restore the saved registers */
    lw      r1, (sp+4)
    lw      r2, (sp+8)
    lw      r3, (sp+12)
    lw      r4, (sp+16)
    lw      r5, (sp+20)
    lw      r6, (sp+24)
    lw      r7, (sp+28)
    lw      r8, (sp+32)
    lw      r9, (sp+36)
    lw      r10, (sp+40)
    lw      ra, (sp+44)
    lw      ea, (sp+48)
    lw      ba, (sp+52)
    addi    sp, sp, 52
    eret

