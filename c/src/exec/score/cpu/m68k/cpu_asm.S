/*  cpu_asm.s
 *
 *  This file contains all assembly code for the MC68020 implementation
 *  of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <asm.h>

        .text

/*  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

        .align  4
        .global SYM (_CPU_Context_switch)

.set RUNCONTEXT_ARG,   4                   | save context argument
.set HEIRCONTEXT_ARG,  8                   | restore context argument

SYM (_CPU_Context_switch):
          moval    a7@(RUNCONTEXT_ARG),a0| a0 = running thread context
          movw     sr,d1                 | d1 = status register
          movml    d1-d7/a2-a7,a0@       | save context

          moval    a7@(HEIRCONTEXT_ARG),a0| a0 = heir thread context
restore:  movml    a0@,d1-d7/a2-a7     | restore context
          movw     d1,sr                  | restore status register
          rts

/*PAGE
 *  void __CPU_Context_save_fp_context( &fp_context_ptr )
 *  void __CPU_Context_restore_fp_context( &fp_context_ptr )
 *
 *  These routines are used to context switch a MC68881 or MC68882.
 *
 *  NOTE:  Context save and restore code is based upon the code shown
 *         on page 6-38 of the MC68881/68882 Users Manual (rev 1).
 *
 *         CPU_FP_CONTEXT_SIZE is higher than expected to account for the
 *         -1 pushed at end of this sequence.
 *
 *         Neither of these entries is required if we have software FPU
 *         emulation.  But if we don't have an FPU or emulation, then
 *         we need the stub versions of these routines.
 */

#if (CPU_SOFTWARE_FP == FALSE)

.set FPCONTEXT_ARG,   4                    | save FP context argument

        .align  4
        .global SYM (_CPU_Context_save_fp)
SYM (_CPU_Context_save_fp):
#if ( M68K_HAS_FPU == 1 )
        moval    a7@(FPCONTEXT_ARG),a1    | a1 = &ptr to context area
        moval    a1@,a0                   | a0 = Save context area
        fsave    a0@-                     | save 68881/68882 state frame
        tstb     a0@                      | check for a null frame
        beq      nosv                     | Yes, skip save of user model
        fmovem   fp0-fp7,a0@-             | save data registers (fp0-fp7)
        fmovem   fpc/fps/fpi,a0@-         | and save control registers
        movl     #-1,a0@-                 | place not-null flag on stack
nosv:   movl     a0,a1@                   | save pointer to saved context
#endif
        rts

        .align  4
        .global SYM (_CPU_Context_restore_fp)
SYM (_CPU_Context_restore_fp):
#if ( M68K_HAS_FPU == 1 )
        moval    a7@(FPCONTEXT_ARG),a1    | a1 = &ptr to context area
        moval    a1@,a0                   | a0 = address of saved context
        tstb     a0@                      | Null context frame?
        beq      norst                    | Yes, skip fp restore
        addql    #4,a0                    | throwaway non-null flag
        fmovem   a0@+,fpc/fps/fpi         | restore control registers
        fmovem   a0@+,fp0-fp7             | restore data regs (fp0-fp7)
norst:  frestore a0@+                     | restore the fp state frame
        movl     a0,a1@                   | save pointer to saved context
#endif
        rts
#endif

/*PAGE
 *  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  NOTE:
 *    Upon entry, the master stack will contain an interrupt stack frame
 *    back to the interrupted thread and the interrupt stack will contain
 *    a throwaway interrupt stack frame.  If dispatching is enabled, this
 *    is the outer most interrupt, and (a context switch is necessary or
 *    the current thread has signals), then set up the master stack to
 *    transfer control to the interrupt dispatcher.
 */

/*
 *  With this approach, lower priority interrupts may
 *  execute twice if a higher priority interrupt is
 *  acknowledged before _Thread_Dispatch_disable is
 *  incremented and the higher priority interrupt
 *  performs a context switch after executing. The lower
 *  priority interrupt will execute (1) at the end of the
 *  higher priority interrupt in the new context if
 *  permitted by the new interrupt level mask, and (2) when
 *  the original context regains the cpu.
 */
 
#if ( M68K_COLDFIRE_ARCH == 1 )
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
        addql   #1,SYM (_Thread_Dispatch_disable_level) | disable multitasking
#if ( M68K_COLDFIRE_ARCH == 0 )
        moveml  d0-d1/a0-a1,a7@-         | save d0-d1,a0-a1
        movew   a7@(SAVED+FVO_OFFSET),d0 | d0 = F/VO
        andl    #0x0fff,d0               | d0 = vector offset in vbr
#else
	lea	a7@(-SAVED),a7
	movm.l  d0-d1/a0-a1,a7@		 | save d0-d1,a0-a1
	movew   a7@(SAVED+FVO_OFFSET),d0 | d0 = F/VO
	andl    #0x0ffc,d0               | d0 = vector offset in vbr
#endif


#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
  #if ( M68K_COLDFIRE_ARCH == 0 )
	movew	sr,d1			| Save status register
	oriw	#0x700,sr		| Disable interrupts
  #else
	move.l  d0,a7@-			| Save d0 value
	move.l  #0x700,d0		| Load in disable ints value
	move.w  sr,d1			| Grab SR
	or.l    d1,d0			| Create new SR
	move.w  d0,sr			| Disable interrupts
	move.l  a7@+,d0			| Restore d0 value
  #endif
  
	tstl	SYM (_ISR_Nest_level)	| Interrupting an interrupt handler?
	bne	1f			| Yes, just skip over stack switch code
	movel	SYM(_CPU_Interrupt_stack_high),a0	| End of interrupt stack
	movel	a7,a0@-			| Save task stack pointer
	movel	a0,a7			| Switch to interrupt stack
1:
	addql   #1,SYM(_ISR_Nest_level)	| one nest level deeper
	movew	d1,sr			| Restore status register
#else
	addql   #1,SYM (_ISR_Nest_level) | one nest level deeper
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */

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

#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
  #if ( M68K_COLDFIRE_ARCH == 0 )
	movew	sr,d0			| Save status register
	oriw	#0x700,sr		| Disable interrupts
  #else
	move.l  #0x700,d1		| Load in disable int value
	move.w  sr,d0			| Grab SR
	or.l    d0,d1			| Create new SR
	move.w  d1,sr			| Load to disable interrupts
  #endif

	subql	#1,SYM(_ISR_Nest_level)	| Reduce interrupt-nesting count
	bne	1f			| Skip if return to interrupt
	movel	(a7),a7			| Restore task stack pointer
1:
	movew	d0,sr			| Restore status register
#else
	subql   #1,SYM (_ISR_Nest_level) | one less nest level
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */

        subql   #1,SYM (_Thread_Dispatch_disable_level)
                                         | unnest multitasking
        bne     exit                     | If dispatch disabled, exit

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movew   #0xf000,d0               | isolate format nibble
        andw    a7@(SAVED+FVO_OFFSET),d0 | get F/VO
        cmpiw   #0x1000,d0               | is it a throwaway isf?
        bne     exit                     | NOT outer level, so branch
#endif

        tstl    SYM (_Context_Switch_necessary)
                                         | Is thread switch necessary?
        bne     bframe                   | Yes, invoke dispatcher

        tstl    SYM (_ISR_Signals_to_thread_executing)
                                         | signals sent to Run_thread
                                         |   while in interrupt handler?
        beq     exit                     | No, then exit


bframe: clrl    SYM (_ISR_Signals_to_thread_executing)
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

#if ( M68K_COLDFIRE_ARCH == 0 )
exit:   moveml  a7@+,d0-d1/a0-a1	 | restore d0-d1,a0-a1
#else
exit:	moveml	a7@,d0-d1/a0-a1		 | restore d0-d1,a0-a1
	lea     a7@(SAVED),a7
#endif

#if ( M68K_HAS_VBR == 0 )
        addql   #2,a7                    | pop format/id
#endif /* M68K_HAS_VBR */
        rte                              | return to thread
                                         |   OR _Isr_dispatch

/*PAGE
 *  void _ISR_Dispatch()
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
#if ( M68K_COLDFIRE_ARCH == 0 )
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
