/* irq_asm.S
 *
 *  This file contains the implementation of the IRQ handler
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *      Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  Modified Andy Dachs <a.dachs@sstl.co.uk>
 *  Copyright (c) 2001 Surrey Satellite Technolgy Limited
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "asm.h"
#define __asm__

        .globl _ISR_Handler
_ISR_Handler:
        stmdb   sp!, {r0, r1, r2, r3, r12}   /* save regs on INT stack */
        stmdb   sp!, {lr}               /*    now safe to call C funcs */
        

/* one nest level deeper */
        ldr     r0, =_ISR_Nest_level 
        ldr     r1, [r0]
        add     r1, r1,#1
        str     r1, [r0]
        
/* disable multitasking */
        ldr     r0, =_Thread_Dispatch_disable_level          
        ldr     r1, [r0]
        add     r1, r1,#1
        str     r1, [r0]

/* BSP specific function to INT handler */
        /* FIXME: I'm not sure why I can't save just r12. I'm also  */
        /*     not sure which of r1-r3 are important.               */
        bl      ExecuteITHandler 

/* one less nest level  */      
        ldr     r0, =_ISR_Nest_level
        ldr     r1, [r0]
        sub     r1, r1,#1
        str     r1, [r0]
        
/* unnest multitasking */
        ldr     r0, =_Thread_Dispatch_disable_level
        ldr     r1, [r0]
        sub     r1, r1,#1
        str     r1, [r0]

/* check to see if we interrupted and INT (with FIQ?) */
        mrs   r0, spsr
        and   r0, r0, #0x1f
        cmp   r0, #0x12        /* is it INT mode? */  
        beq   exitit

/* check to see if we interrupted nd INT (with FIQ?) */
        mrs   r0, spsr
        and   r0, r0, #0x1f
        cmp   r0, #0x12        /* is it INT mode? */  
        beq   exitit

/* If thread dispatching is disabled, exit */
        cmp     r1, #0
        bne     exitit

/* If a task switch is necessary, call scheduler */
        ldr     r0, =_Context_Switch_necessary
        ldr     r1, [r0]
        cmp     r1, #0
        
        /* since bframe is going to clear _ISR_Signals_to_thread_executing, */
        /*    we need to load it here */
        ldr     r0, =_ISR_Signals_to_thread_executing   
        ldr     r1, [r0]
        bne     bframe
        
/* If a signals to be sent (_ISR_Signals_to_thread_executing != 0),        */
/*  call scheduler */
        cmp     r1, #0
        beq     exitit
        
/* _ISR_Signals_to_thread_executing = FALSE */
        mov     r1, #0
        str     r1, [r0]

bframe:

/* Now we need to set up the return from this ISR to be _ISR_Dispatch */
/* To do that, we need to save the current lr_int and spsr_int on the */
/* SVC stack                                                          */
        mrs     r0, spsr
        ldmia   sp!, {r1}       /* get lr off stack */
        stmdb   sp!, {r1}
        mrs     r2, cpsr    
        orr     r3, r2, #0x1    /* change to SVC mode */
        msr     cpsr_c, r3

        /* now in SVC mode */
        stmdb   sp!, {r0, r1}   /* put spsr_int and lr_int on SVC stack */
        msr     cpsr_c, r2      /* change back to INT mode */

        /* now in INT mode */

        /* replace lr with address of _ISR_Dispatch */
        ldr     lr, =_ISR_Dispatch_p_4    /* On entry to an ISR, the lr is */
                                          /*    the return address + 4, so */
                                          /*    we have to emulate that    */
        ldmia   sp!, {r1}                 /* out with the old          */
        stmdb   sp!, {lr}                 /*    in with the new (lr) */

        
        orr     r0, r0, #0xc0
        msr     spsr, r0
                                        
exitit:
        ldmia   sp!, {lr}                     /* restore regs from INT stack */
        ldmia   sp!, {r0, r1, r2, r3, r12}    /* restore regs from INT stack */
        subs    pc, lr, #4                /* return */



        /* on entry to _ISR_Dispatch, we're in SVC mode */      
        .globl _ISR_Dispatch
_ISR_Dispatch:
        stmdb   sp!, {r0-r3, r12,lr}      /* save regs on SVC stack */
                                          /*    (now safe to call C funcs) */
                                          /*    we don't save lr, since  */
                                          /*    it's just going to get   */
                                          /*    overwritten              */
_ISR_Dispatch_p_4:	
        bl      _Thread_Dispatch
	ldmia	sp!, {r0-r3, r12, lr}

	stmdb   sp!, {r0-r2}
        /* Now we have to screw with the stack */
        mov     r0, sp                  /* copy the SVC stack pointer */
        
        mrs     r1, cpsr    
        bic     r2, r1, #0x1            /* change to INT mode */
        orr     r2, r2, #0xc0           /* disable interrupts */
        msr     cpsr_c, r2

        /* now in INT mode */
        stmdb   sp!, {r4, r5, r6}   /* save temp vars on INT stack */
        ldmia   r0!, {r4, r5, r6}   /* Get r0-r3 from SVC stack */
        stmdb   sp!, {r4, r5, r6}   /*    and save them on INT stack */
        
        ldmia   r0!, {r4, r5}           /* get saved values from SVC stack */
                                        /*      r4=spsr, r5=lr */
        mov     lr,   r5                /* restore lr_int */
        msr     spsr, r4                /* restore spsr_int */

        /* switch to SVC mode, update sp, then return to INT mode */
        msr     cpsr_c, r1              /* switch to SVC mode */
        mov     sp, r0                  /* update sp_svc */
        msr     cpsr_c, r2              /* switch back to INT mode */

        /* pop all the registers from the stack */
        ldmia   sp!, {r0, r1, r2}
        ldmia   sp!, {r4, r5, r6}

        /* Finally, we can return to the interrupted task */
        subs    pc, lr, #4





