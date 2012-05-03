/*  cpu_asm.s
 *
 *  This file contains all assembly code for the MC68020 implementation
 *  of RTEMS.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

/*  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

        .align  4
        .global SYM (_CPU_Context_switch)

.set RUNCONTEXT_ARG,   4                 | save context argument
.set HEIRCONTEXT_ARG,  8                 | restore context argument

SYM (_CPU_Context_switch):
          moval    a7@(RUNCONTEXT_ARG),a0| a0 = running thread context
          movw     sr,d1                 | d1 = status register
          movml    d1-d7/a2-a7,a0@       | save context

          moval    a7@(HEIRCONTEXT_ARG),a0| a0 = heir thread context

#if defined( __mcoldfire__ ) && ( M68K_HAS_FPU == 1 )
          moveb    a0@(13*4),d0                 | get context specific DF bit info in d0
          btstb    #4,d0                        | test context specific DF bit info
          beq      fpu_on                       | branch if FPU needs to be switched on

fpu_off:  movl     _CPU_cacr_shadow,d0          | get content of _CPU_cacr_shadow in d0
          btstl    #4,d0                        | test DF bit info in d0
          bne      restore                      | branch if FPU is already switched off
          bsetl    #4,d0                        | set DF bit in d0
          bra      cacr_set                     | branch to set the new FPU setting in cacr and _CPU_cacr_shadow

fpu_on:   movl     _CPU_cacr_shadow,d0          | get content of _CPU_cacr_shadow in d1
          btstl    #4,d0                        | test context specific DF bit info
          beq      restore                      | branch if FPU is already switched on
          bclrl    #4,d0                        | clear DF bit info in d0

cacr_set: movew    sr,d1                        | get content of sr in d1
          oril     #0x00000700,d1               | mask d1
          movew    d1,sr                        | disable all interrupts
          movl     d0,_CPU_cacr_shadow          | move _CPU_cacr_shadow to d1
          movec    d0,cacr                      | enable FPU in cacr
#endif


restore:  movml    a0@,d1-d7/a2-a7       | restore context
          movw     d1,sr                 | restore status register
          rts

        .global SYM (_CPU_Context_Restart_self)
.set    CONTEXT_ARG, 4                   | context arg

#if defined( __mcoldfire__ ) && ( M68K_HAS_FPU == 1 )
/* XXX _CPU_Context_switch maintains FPU context -- do we have to restore
 *     that, too??
 */
#warning "_CPU_Context_Restart_self restoring FPU context not implemented"
#endif
SYM(_CPU_Context_Restart_self):
         moval    a7@(CONTEXT_ARG),a0
         bra      restore
/*
 * Floating point context save and restore.
 *
 * The code for the MC68881 or MC68882 is based upon the code shown on pages
 * 6-38 of the MC68881/68882 Users Manual (rev 1).  CPU_FP_CONTEXT_SIZE is
 * higher than expected to account for the -1 pushed at end of this sequence.
 */

#if ( CPU_HARDWARE_FP == TRUE )

.set FPCONTEXT_ARG,   4                   | save FP context argument

        .align  4
        .global SYM (_CPU_Context_save_fp)
SYM (_CPU_Context_save_fp):

        /* Get context save area pointer argument from the stack */
        moval    a7@(FPCONTEXT_ARG), a1
        moval    a1@, a0

  #if defined( __mcoldfire__ )
        /* Move MACSR to data register and disable rounding */
        movel    macsr, d0
        clrl     d1
        movl     d1, macsr

        /* Save MACSR and ACC0 */
        movl     acc0, d1
        moveml   d0-d1, a0@(0)

        /* Save ACC1 and ACC2 */
        movl     acc1, d0
        movl     acc2, d1
        moveml   d0-d1, a0@(8)

        /* Save ACC3 and ACCEXT01 */
        movl     acc3, d0
        movl     accext01, d1
        moveml   d0-d1, a0@(16)

        /* Save ACCEXT23 and MASK */
        movl     accext23, d0
        movl     mask, d1
        moveml   d0-d1, a0@(24)

    #if ( M68K_HAS_FPU == 1 )
        /* Save FP state */
        fsave    a0@(32)

        /* Save FP instruction address */
        fmovel   fpi, a0@(48)

        /* Save FP data */
        fmovem   fp0-fp7, a0@(52)
    #endif
  #else
    #if defined( __mc68060__ )
        lea      a0@(-M68K_FP_STATE_SIZE), a0
        fsave    a0@                      | save 68060 state frame
    #else
        fsave    a0@-                     | save 68881/68882 state frame
    #endif
        tstb     a0@                      | check for a null frame
        beq.b    nosv                     | Yes, skip save of user model
        fmovem   fp0-fp7, a0@-            | save data registers (fp0-fp7)
        fmovem   fpc/fps/fpi, a0@-        | and save control registers
        movl     #-1, a0@-                | place not-null flag on stack
nosv:
        movl     a0, a1@                  | save pointer to saved context
  #endif

        /* Return */
        rts

        .align  4
        .global SYM (_CPU_Context_restore_fp)
SYM (_CPU_Context_restore_fp):

        /* Get context save area pointer argument from the stack */
        moval    a7@(FPCONTEXT_ARG), a1
        moval    a1@, a0

  #if defined( __mcoldfire__ )
    #if ( M68K_HAS_FPU == 1 )
        /* Restore FP data */
        fmovem   a0@(52), fp0-fp7

        /* Restore FP instruction address */
        fmovel   a0@(48), fpi

        /* Restore FP state */
        frestore a0@(32)
    #endif

        /* Disable rounding */
        clrl     d0
        movl     d0, macsr

        /* Restore MASK and ACCEXT23 */
        moveml   a0@(24), d0-d1
        movl     d0, mask
        movl     d1, accext23

        /* Restore ACCEXT01 and ACC3 */
        moveml   a0@(16), d0-d1
        movl     d0, accext01
        movl     d1, acc3

        /* Restore ACC2 and ACC1 */
        moveml   a0@(8), d0-d1
        movl     d0, acc2
        movl     d1, acc1

        /* Restore ACC0 and MACSR */
        moveml   a0@(0), d0-d1
        movl     d0, acc0
        movl     d1, macsr
  #else
        tstb     a0@                      | Null context frame?
        beq.b    norst                    | Yes, skip fp restore
        addql    #4, a0                   | throwaway non-null flag
        fmovem   a0@+, fpc/fps/fpi        | restore control registers
        fmovem   a0@+, fp0-fp7            | restore data regs (fp0-fp7)
norst:
    #if defined( __mc68060__ )
        frestore a0@                      | restore 68060 state frame
        lea      a0@(M68K_FP_STATE_SIZE), a0
    #else
        frestore a0@+                     | restore 68881/68882 state frame
    #endif
        movl     a0, a1@                  | save pointer to saved context
  #endif

        /* Return */
        rts
#endif

/*void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  NOTE:
 *    Upon entry, the master stack will contain an interrupt stack frame
 *    back to the interrupted thread and the interrupt stack will contain
 *    a throwaway interrupt stack frame.  If dispatching is enabled, and this
 *    is the outer most interrupt, and a context switch is necessary or
 *    the current thread has pending signals, then set up the master stack to
 *    transfer control to the interrupt dispatcher.
 */

#if ( defined(__mcoldfire__) )
.set SR_OFFSET,    2                     | Status register offset
.set PC_OFFSET,    4                     | Program Counter offset
.set FVO_OFFSET,   0                     | Format/vector offset
#elif ( M68K_HAS_VBR == 1)
.set SR_OFFSET,    0                     | Status register offset
.set PC_OFFSET,    2                     | Program Counter offset
.set FVO_OFFSET,   6                     | Format/vector offset
#else
.set SR_OFFSET,    2                     | Status register offset
.set PC_OFFSET,    4                     | Program Counter offset
.set FVO_OFFSET,   0                     | Format/vector offset placed in the stack
#endif /* M68K_HAS_VBR */

.set SAVED,        16                    | space for saved registers

        .align  4
        .global SYM (_ISR_Handler)

SYM (_ISR_Handler):
                                         | disable multitasking
        addql   #1,SYM (_Thread_Dispatch_disable_level)
#if ( !defined(__mcoldfire__) )
        moveml  d0-d1/a0-a1,a7@-         | save d0-d1,a0-a1
#else
        lea     a7@(-SAVED),a7
        movm.l  d0-d1/a0-a1,a7@          | save d0-d1,a0-a1
#endif
        movew   a7@(SAVED+FVO_OFFSET),d0 | d0 = F/VO
        andl    #0x03fc,d0               | d0 = vector offset in vbr


#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
        | Make a0 point just above interrupt stack
        movel   INTERRUPT_STACK_HIGH,a0
        cmpl    INTERRUPT_STACK_LOW,a7  | stack below interrupt stack?
        bcs.b   1f                      | yes, switch to interrupt stack
        cmpl    a0,a7                   | stack above interrupt stack?
        bcs.b   2f                      | no, do not switch stacks
1:
        movel   a7,a1                   | copy task stack pointer
        movel   a0,a7                   | switch to interrupt stack
        movel   a1,a7@-                 | store task stack pointer
                                        |     on interrupt stack
2:
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */

        addql   #1,ISR_NEST_LEVEL        | one nest level deeper

        movel   SYM (_ISR_Vector_table),a0 | a0= base of RTEMS table
#if ( M68K_HAS_PREINDEXING == 1 )
        movel   (a0,d0:w:1),a0           | a0 = address of user routine
#else
        addal   d0,a0                    | a0 = address of vector
        movel   (a0),a0                  | a0 = address of user routine
#endif

        lsrl    #2,d0                    | d0 = vector number
        movel   d0,a7@-                  | push vector number
        jbsr    a0@                      | invoke the user ISR
        addql   #4,a7                    | remove vector number
        subql   #1,ISR_NEST_LEVEL        | Reduce interrupt-nesting count

#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
        movel   INTERRUPT_STACK_HIGH,a0
        subql   #4,a0
        cmpl    a0,a7                   | At top of interrupt stack?
        bne.b   1f                      | No, do not restore task stack pointer
        movel   (a7),a7                 | Restore task stack pointer
1:
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */
        subql   #1,SYM (_Thread_Dispatch_disable_level)
                                         | unnest multitasking
        bne.b    exit                    | If dispatch disabled, exit

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movew   #0xf000,d0               | isolate format nibble
        andw    a7@(SAVED+FVO_OFFSET),d0 | get F/VO
        cmpiw   #0x1000,d0               | is it a throwaway isf?
        bne.b   exit                     | NOT outer level, so branch
#else
/*
 * If we have a CPU which allows a higher-priority interrupt to preempt a
 * lower priority handler before the lower-priority handler can increment
 * _Thread_Dispatch_disable_level then we must check the PC on the stack to
 * see if it is _ISR_Handler.  If it is we have the case of nesting interrupts
 * without the dispatch level being incremented.
 */
  #if ( !defined(__mcoldfire__) && !__mc68060__ )
        cmpl    #_ISR_Handler,a7@(SAVED+PC_OFFSET)
        beq.b   exit
  #endif
#endif
        tstb    DISPATCH_NEEDED
                                         | Is thread switch necessary?
        beq.b   exit                     | No, then exit

bframe: 
                                         | If sent, will be processed
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movec   msp,a0                   | a0 = master stack pointer
        movew   #0,a0@-                  | push format word
        movel   #SYM(_ISR_Dispatch),a0@- | push return addr
        movew   a0@(6),a0@-              | push saved sr
        movec   a0,msp                   | set master stack pointer
#else
        jsr SYM (_Thread_Dispatch)       | Perform context switch
#endif

#if ( !defined(__mcoldfire__) )
exit:   moveml  a7@+,d0-d1/a0-a1         | restore d0-d1,a0-a1
#else
exit:   moveml  a7@,d0-d1/a0-a1          | restore d0-d1,a0-a1
        lea     a7@(SAVED),a7
#endif

#if ( M68K_HAS_VBR == 0 )
        addql   #2,a7                    | pop format/id
#endif /* M68K_HAS_VBR */
        rte                              | return to thread
                                         |   OR _Isr_dispatch

/*void _ISR_Dispatch()
 *
 *  Entry point from the outermost interrupt service routine exit.
 *  The current stack is the supervisor mode stack if this processor
 *  has separate stacks.
 *
 *    1.  save all registers not preserved across C calls.
 *    2.  invoke the _Thread_Dispatch routine to switch tasks
 *        or a signal to the currently executing task.
 *    3.  restore all registers not preserved across C calls.
 *    4.  return from interrupt
 */

        .global SYM (_ISR_Dispatch)
SYM (_ISR_Dispatch):
#if ( !defined(__mcoldfire__) )
        movml   d0-d1/a0-a1,a7@-
        jsr     SYM (_Thread_Dispatch)
        movml   a7@+,d0-d1/a0-a1
#else
        lea     a7@(-SAVED),a7
        movml   d0-d1/a0-a1,a7@
        jsr     SYM (_Thread_Dispatch)
        movml   a7@,d0-d1/a0-a1
        lea     a7@(SAVED),a7
#endif

#if ( M68K_HAS_VBR == 0 )
        addql   #2,a7                    | pop format/id
#endif /* M68K_HAS_VBR */
        rte
