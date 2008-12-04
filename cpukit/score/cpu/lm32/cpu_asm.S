/*
 *  $Id$
 *
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
 */

#include <rtems/asm.h>
#include <rtems/score/cpu_asm.h>

#define MICO32_FULL_CONTEXT_SAVE_RESTORE
	
/*  void _CPU_Context_switch(run_context, heir_context)
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  LM32 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
    .globl _CPU_Context_switch
_CPU_Context_switch:
    sw      (r1+0), r9
    sw      (r1+4), r10
    sw      (r1+8), r11
    sw      (r1+12), r12
    sw      (r1+16), r13
    sw      (r1+20), r14
    sw      (r1+24), r15
    sw      (r1+28), r16
    sw      (r1+32), r17
    sw      (r1+36), r18
    sw      (r1+40), r19
    sw      (r1+44), r20
    sw      (r1+48), r21
    sw      (r1+52), r22
    sw      (r1+56), r23
    sw      (r1+60), r24
    sw      (r1+64), r25
    sw      (r1+68), gp
    sw      (r1+72), fp
    sw      (r1+76), sp
    sw      (r1+80), ra

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 *  LM32 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
    .globl _CPU_Context_restore
_CPU_Context_restore:
    lw      r9, (r2+0)
    lw      r10, (r2+4)
    lw      r11, (r2+8)
    lw      r12, (r2+12)
    lw      r13, (r2+16)
    lw      r14, (r2+20)
    lw      r15, (r2+24)
    lw      r16, (r2+28)
    lw      r17, (r2+32)
    lw      r18, (r2+36)
    lw      r19, (r2+40)
    lw      r20, (r2+44)
    lw      r21, (r2+48)
    lw      r22, (r2+52)
    lw      r23, (r2+56)
    lw      r24, (r2+60)
    lw      r25, (r2+64)
    lw      gp, (r2+68)
    lw      fp, (r2+72)
    lw      ra, (r2+80)
    /* Stack pointer must be restored last, in case it has been updated */
    lw      sp, (r2+76)
    nop
    ret
	
/*  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  LM32 Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

    .globl  _ISR_Handler
_ISR_Handler:
    addi    sp, sp, -128
    sw      (sp+4), r1
    sw      (sp+8), r2
    sw      (sp+12), r3
    sw      (sp+16), r4
    sw      (sp+20), r5
    sw      (sp+24), r6
    sw      (sp+28), r7
    sw      (sp+32), r8
    sw      (sp+36), r9
    sw      (sp+40), r10
#ifdef MICO32_FULL_CONTEXT_SAVE_RESTORE
    sw      (sp+44), r11
    sw      (sp+48), r12
    sw      (sp+52), r13
    sw      (sp+56), r14
    sw      (sp+60), r15
    sw      (sp+64), r16
    sw      (sp+68), r17
    sw      (sp+72), r18
    sw      (sp+76), r19
    sw      (sp+80), r20
    sw      (sp+84), r21
    sw      (sp+88), r22
    sw      (sp+92), r23
    sw      (sp+96), r24
    sw      (sp+100), r25
    sw      (sp+104), r26
    sw      (sp+108), r27
#endif
    sw      (sp+116), ra
    sw      (sp+120), ea
    sw      (sp+124), ba

    /* Scan through (IP & IM) bits starting from LSB until irq found */ 
    rcsr    r2,IP
    rcsr    r3,IM
    and     r4,r2,r3
    or      r1,r0,r0
    ori	    r5,r0,1
find_irq:	
    and     r6,r4,r5
    bne     r6,r0,found_irq
    sli     r5,r5,1
    addi    r1,r1,1
    bne     r5,r0,find_irq
    /* If we end up here there was no interrupt - this should never
       happen! */
    bi      exit_isr

found_irq:
    .extern _ISR_Vector_table
    sli     r1,r1,2
    mvhi    r7,hi(_ISR_Vector_table)
    ori     r7,r7,lo(_ISR_Vector_table)
    lw      r6,(r7+0)
    add	    r6,r6,r1
    lw      r5,(r6+0)
    call    r5

exit_isr:
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
#ifdef MICO32_FULL_CONTEXT_SAVE_RESTORE
    lw      r11, (sp+44)
    lw      r12, (sp+48)
    lw      r13, (sp+52)
    lw      r14, (sp+56)
    lw      r15, (sp+60)
    lw      r16, (sp+64)
    lw      r17, (sp+68)
    lw      r18, (sp+72)
    lw      r19, (sp+76)
    lw      r20, (sp+80)
    lw      r21, (sp+84)
    lw      r22, (sp+88)
    lw      r23, (sp+92)
    lw      r24, (sp+96)
    lw      r25, (sp+100)
    lw      r26, (sp+104)
    lw      r27, (sp+108)
#endif
    lw      ra, (sp+116)
    lw      ea, (sp+120)
    lw      ba, (sp+124)
    addi    sp, sp, 128
    nop
    eret

