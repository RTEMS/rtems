/* 
 * TODO: 
 *       Context_switch needs to only save callee save registers
 *       I think this means can skip:    r1, r2, r19-29, r31
 *       Ref:     p 3-2 of Procedure Calling Conventions Manual
 *       This should be #ifndef DEBUG so that debugger has
 *       accurate visibility into all registers
 *
 *  This file contains the assembly code for the HPPA implementation
 *  of RTEMS.
 *
 *  COPYRIGHT (c) 1994,95 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/score/hppa.h>
#include <rtems/score/cpu_asm.h>
#include <rtems/score/cpu.h>
#include <rtems/score/offsets.h>

	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY
	.SPACE $TEXT$
	.SUBSPA $CODE$

/*
 * Special register usage for context switch and interrupts
 * Stay away from %cr28 which is used for TLB misses on 72000
 */

isr_arg0           .reg    %cr24
isr_r9             .reg    %cr25
isr_r8             .reg    %cr26

/*
 * Interrupt stack frame looks like this
 *
 *  offset                                   item
 * -----------------------------------------------------------------
 *   INTEGER_CONTEXT_OFFSET             Context_Control
 *   FP_CONTEXT_OFFSET                  Context_Control_fp
 *
 * It is padded out to a multiple of 64
 */


/*PAGE^L
 *  void _Generic_ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *   We jump here from the interrupt vector.
 *   The HPPA hardware has done some stuff for us:
 *       PSW saved in IPSW
 *       PSW set to 0
 *       PSW[E] set to default (0)
 *       PSW[M] set to 1 iff this is HPMC
 *
 *       IIA queue is frozen (since PSW[Q] is now 0)
 *       privilege level promoted to 0
 *       IIR, ISR, IOR potentially updated if PSW[Q] was 1 at trap
 *       registers GR  1,8,9,16,17,24,25 copied to shadow regs
 *                 SHR 0 1 2  3  4  5  6
 *
 *   Our vector stub (in the BSP) MUST have done the following:  
 *
 *   a) Saved the original %r9 into %isr_r9 (%cr25)
 *   b) Placed the vector number in %r9
 *   c) Was allowed to also destroy $isr_r8 (%cr26),
 *      but the stub was NOT allowed to destroy any other registers.
 *
 *   The typical stub sequence (in the BSP) should look like this:
 *
 *   a)     mtctl   %r9,isr_r9     ; (save r9 in cr25)
 *   b)     ldi     vector,%r9     ; (load constant vector number in r9)
 *   c)     mtctl   %r8,isr_r8     ; (save r8 in cr26)
 *   d)     ldil    L%MY_BSP_first_level_interrupt_handler,%r8
 *   e)     ldo     R%MY_BSP_first_level_interrupt_handler(%r8),%r8
 *                                 ; (point to BSP raw handler table)
 *   f)     ldwx,s  %r9(%r8),%r8   ; (load value from raw handler table)
 *   g)     bv      0(%r8)         ; (call raw handler: _Generic_ISR_Handler)
 *   h)     mfctl   isr_r8,%r8     ; (restore r8 from cr26 in delay slot)
 *
 *   Optionally, steps (c) thru (h) _could_ be replaced with a single
 *          bl,n    _Generic_ISR_Handler,%r0
 *
 *
 */
	.EXPORT _Generic_ISR_Handler,ENTRY,PRIV_LEV=0
_Generic_ISR_Handler:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY

        mtctl     arg0, isr_arg0

/*
 * save interrupt state
 */
        mfctl     ipsw, arg0
        stw       arg0, IPSW_OFFSET(sp)

        mfctl     iir, arg0
        stw       arg0, IIR_OFFSET(sp)

        mfctl     ior, arg0
        stw       arg0, IOR_OFFSET(sp)

        mfctl     pcoq, arg0
        stw       arg0, PCOQFRONT_OFFSET(sp)

	mtctl	  %r0, pcoq
        mfctl     pcoq, arg0
        stw       arg0, PCOQBACK_OFFSET(sp)

        mfctl     %sar, arg0
        stw       arg0, SAR_OFFSET(sp)

/*
 * Build an interrupt frame to hold the contexts we will need.
 * We have already saved the interrupt items on the stack
 *
 * At this point the following registers are damaged wrt the interrupt
 *  reg    current value        saved value
 * ------------------------------------------------
 *  arg0   scratch               isr_arg0  (cr24)
 *  r9     vector number         isr_r9    (cr25)
 *
 * Point to beginning of integer context and
 * save the integer context
 */
        stw         %r1,R1_OFFSET(sp)
        stw         %r2,R2_OFFSET(sp)
        stw         %r3,R3_OFFSET(sp)
        stw         %r4,R4_OFFSET(sp)
        stw         %r5,R5_OFFSET(sp)
        stw         %r6,R6_OFFSET(sp)
        stw         %r7,R7_OFFSET(sp)
        stw         %r8,R8_OFFSET(sp)
/*
 * skip r9
 */
        stw         %r10,R10_OFFSET(sp)
        stw         %r11,R11_OFFSET(sp)
        stw         %r12,R12_OFFSET(sp)
        stw         %r13,R13_OFFSET(sp)
        stw         %r14,R14_OFFSET(sp)
        stw         %r15,R15_OFFSET(sp)
        stw         %r16,R16_OFFSET(sp)
        stw         %r17,R17_OFFSET(sp)
        stw         %r18,R18_OFFSET(sp)
        stw         %r19,R19_OFFSET(sp)
        stw         %r20,R20_OFFSET(sp)
        stw         %r21,R21_OFFSET(sp)
        stw         %r22,R22_OFFSET(sp)
        stw         %r23,R23_OFFSET(sp)
        stw         %r24,R24_OFFSET(sp)
        stw         %r25,R25_OFFSET(sp)
/*
 * skip arg0
 */
        stw         %r27,R27_OFFSET(sp)
        stw         %r28,R28_OFFSET(sp)
        stw         %r29,R29_OFFSET(sp)
        stw         %r30,R30_OFFSET(sp)
        stw         %r31,R31_OFFSET(sp)

/* Now most registers are available since they have been saved
 *
 * The following items are currently wrong in the integer context
 *  reg    current value        saved value
 * ------------------------------------------------
 *  arg0   scratch               isr_arg0  (cr24)
 *  r9     vector number         isr_r9    (cr25)
 *
 * Fix them
 */

         mfctl      isr_arg0,%r3
         stw        %r3,ARG0_OFFSET(sp)

         mfctl      isr_r9,%r3
         stw        %r3,R9_OFFSET(sp)

/*
 * At this point we are done with isr_arg0, and isr_r9 control registers
 *
 * Prepare to re-enter virtual mode
 * We need Q in case the interrupt handler enables interrupts
 */

        ldil      L%CPU_PSW_DEFAULT, arg0
        ldo       R%CPU_PSW_DEFAULT(arg0), arg0
        mtctl     arg0, ipsw

/*
 * Now jump to "rest_of_isr_handler" with the rfi
 * We are assuming the space queues are all correct already
 */

	ldil 	  L%rest_of_isr_handler, arg0
	ldo	  R%rest_of_isr_handler(arg0), arg0
	mtctl	  arg0, pcoq
	ldo	  4(arg0), arg0
	mtctl	  arg0, pcoq

        rfi
	nop

/*
 * At this point we are back in virtual mode and all our
 *  normal addressing is once again ok.
 *
 *  It is now ok to take an exception or trap
 */

rest_of_isr_handler:

/*
 * Point to beginning of float context and
 * save the floating point context -- doing whatever patches are necessary
 */

        .call ARGW0=GR
        bl          _CPU_Save_float_context,%r2
        ldo         FP_CONTEXT_OFFSET(sp),arg0

/*
 * save the ptr to interrupt frame as an argument for the interrupt handler
 */

        copy        sp, arg1

/*
 * Advance the frame to point beyond all interrupt contexts (integer & float)
 * this also includes the pad to align to 64byte stack boundary
 */
        ldo         CPU_INTERRUPT_FRAME_SIZE(sp), sp

/*
 *    r3  -- &_ISR_Nest_level
 *    r5  -- value _ISR_Nest_level
 *    r4  -- &_Thread_Dispatch_disable_level
 *    r6  -- value _Thread_Dispatch_disable_level
 *    r9  -- vector number
 */

 	.import   _ISR_Nest_level,data
	ldil	  L%_ISR_Nest_level,%r3
	ldo	  R%_ISR_Nest_level(%r3),%r3
	ldw	  0(%r3),%r5

	.import   _Thread_Dispatch_disable_level,data
	ldil	  L%_Thread_Dispatch_disable_level,%r4
	ldo	  R%_Thread_Dispatch_disable_level(%r4),%r4
	ldw	  0(%r4),%r6

/*
 * increment interrupt nest level counter.  If outermost interrupt
 * switch the stack and squirrel away the previous sp.
 */
        addi      1,%r5,%r5
        stw       %r5, 0(%r3)

/*
 * compute and save new stack (with frame)
 * just in case we are nested -- simpler this way
 */
        comibf,=  1,%r5,stack_done
        ldo       128(sp),%r7

/*
 * Switch to interrupt stack allocated by the interrupt manager (intr.c)
 */
        .import   _CPU_Interrupt_stack_low,data
	ldil	  L%_CPU_Interrupt_stack_low,%r7
	ldw	  R%_CPU_Interrupt_stack_low(%r7),%r7
        ldo       128(%r7),%r7

stack_done:
/*
 * save our current stack pointer where the "old sp" is supposed to be
 */
        stw       sp, -4(%r7)
/*
 * and switch stacks (or advance old stack in nested case)
 */
        copy      %r7, sp

/*
 * increment the dispatch disable level counter.
 */
        addi      1,%r6,%r6
        stw       %r6, 0(%r4)

/*
 * load address of user handler
 * Note:  No error checking is done, it is assumed that the
 *        vector table contains a valid address or a stub
 *        spurious handler.
 */
        .import   _ISR_Vector_table,data
	ldil	  L%_ISR_Vector_table,%r8
	ldo	  R%_ISR_Vector_table(%r8),%r8
        ldw       (%r8),%r8
        ldwx,s    %r9(%r8),%r8

/*
 * invoke user interrupt handler
 * Interrupts are currently disabled, as per RTEMS convention
 * The handler has the option of re-enabling interrupts
 * NOTE:  can not use 'bl' since it uses "pc-relative" addressing
 *    and we are using a hard coded address from a table
 *  So... we fudge r2 ourselves (ala dynacall)
 *  arg0 = vector number, arg1 = ptr to rtems_interrupt_frame
 */
        copy      %r9, %r26
        .call  ARGW0=GR, ARGW1=GR
        blr       %r0, rp
        bv,n      0(%r8)

post_user_interrupt_handler:

/*
 * Back from user handler(s)
 * Disable external interrupts (since the interrupt handler could
 * have turned them on) and return to the interrupted task stack (assuming
 * (_ISR_Nest_level == 0)
 */

        rsm        HPPA_PSW_I + HPPA_PSW_R, %r0
        ldw        -4(sp), sp

/*
 *    r3  -- (most of) &_ISR_Nest_level
 *    r5  -- value _ISR_Nest_level
 *    r4  -- (most of) &_Thread_Dispatch_disable_level
 *    r6  -- value _Thread_Dispatch_disable_level
 *    r7  -- (most of) &_ISR_Signals_to_thread_executing
 *    r8  -- value _ISR_Signals_to_thread_executing
 */

 	.import   _ISR_Nest_level,data
	ldil	  L%_ISR_Nest_level,%r3
	ldw	  R%_ISR_Nest_level(%r3),%r5

	.import   _Thread_Dispatch_disable_level,data
	ldil	  L%_Thread_Dispatch_disable_level,%r4
	ldw	  R%_Thread_Dispatch_disable_level(%r4),%r6

	.import    _ISR_Signals_to_thread_executing,data
	ldil	   L%_ISR_Signals_to_thread_executing,%r7

/*
 * decrement isr nest level
 */
	addi      -1, %r5, %r5
        stw       %r5, R%_ISR_Nest_level(%r3)

/*
 * decrement dispatch disable level counter and, if not 0, go on
 */
        addi       -1,%r6,%r6
        comibf,=   0,%r6,isr_restore
        stw        %r6, R%_Thread_Dispatch_disable_level(%r4)

/*
 * check whether or not a context switch is necessary
 */
	.import    _Context_Switch_necessary,data
	ldil	   L%_Context_Switch_necessary,%r8
	ldw	   R%_Context_Switch_necessary(%r8),%r8
	comibf,=,n 0,%r8,ISR_dispatch

/*
 * check whether or not a context switch is necessary because an ISR
 *    sent signals to the interrupted task
 */
	ldw	   R%_ISR_Signals_to_thread_executing(%r7),%r8
	comibt,=,n 0,%r8,isr_restore


/*
 * OK, something happened while in ISR and we need to switch to a task
 * other than the one which was interrupted or the
 *    ISR_Signals_to_thread_executing case
 * We also turn on interrupts, since the interrupted task had them
 *   on (obviously :-) and Thread_Dispatch is happy to leave ints on.
 */

ISR_dispatch:
	stw	   %r0, R%_ISR_Signals_to_thread_executing(%r7)

        ssm        HPPA_PSW_I, %r0

        .import    _Thread_Dispatch,code
        .call
        bl         _Thread_Dispatch,%r2
        ldo        128(sp),sp

        ldo        -128(sp),sp

isr_restore:

/*
 * enable interrupts during most of restore
 */
        ssm        HPPA_PSW_I, %r0

/*
 * Get a pointer to beginning of our stack frame
 */
        ldo        -CPU_INTERRUPT_FRAME_SIZE(sp), %arg1

/*
 * restore float
 */
        .call ARGW0=GR
        bl         _CPU_Restore_float_context,%r2
        ldo        FP_CONTEXT_OFFSET(%arg1), arg0

        copy       %arg1, %arg0

/*
 *   ********** FALL THRU **********
 */

/*
 * Jump here from bottom of Context_Switch
 * Also called directly by _CPU_Context_Restart_self via _Thread_Restart_self
 * restore interrupt state
 */

	.EXPORT _CPU_Context_restore
_CPU_Context_restore:

/*
 * restore integer state
 */
        ldw         R1_OFFSET(arg0),%r1
        ldw         R2_OFFSET(arg0),%r2
        ldw         R3_OFFSET(arg0),%r3
        ldw         R4_OFFSET(arg0),%r4
        ldw         R5_OFFSET(arg0),%r5
        ldw         R6_OFFSET(arg0),%r6
        ldw         R7_OFFSET(arg0),%r7
        ldw         R8_OFFSET(arg0),%r8
        ldw         R9_OFFSET(arg0),%r9
        ldw         R10_OFFSET(arg0),%r10
        ldw         R11_OFFSET(arg0),%r11
        ldw         R12_OFFSET(arg0),%r12
        ldw         R13_OFFSET(arg0),%r13
        ldw         R14_OFFSET(arg0),%r14
        ldw         R15_OFFSET(arg0),%r15
        ldw         R16_OFFSET(arg0),%r16
        ldw         R17_OFFSET(arg0),%r17
        ldw         R18_OFFSET(arg0),%r18
        ldw         R19_OFFSET(arg0),%r19
        ldw         R20_OFFSET(arg0),%r20
        ldw         R21_OFFSET(arg0),%r21
        ldw         R22_OFFSET(arg0),%r22
        ldw         R23_OFFSET(arg0),%r23
        ldw         R24_OFFSET(arg0),%r24
/*
 * skipping r25; used as scratch register below
 * skipping r26 (arg0) until we are done with it
 */
        ldw         R27_OFFSET(arg0),%r27
        ldw         R28_OFFSET(arg0),%r28
        ldw         R29_OFFSET(arg0),%r29
/*
 * skipping r30 (sp) until we turn off interrupts
 */
        ldw         R31_OFFSET(arg0),%r31

/*
 * Turn off Q & R & I so we can write r30 and interrupt control registers
 */
        rsm        HPPA_PSW_Q + HPPA_PSW_R + HPPA_PSW_I, %r0

/*
 * now safe to restore r30
 */
        ldw         R30_OFFSET(arg0),%r30

        ldw        IPSW_OFFSET(arg0), %r25
        mtctl      %r25, ipsw

        ldw        SAR_OFFSET(arg0), %r25
        mtctl      %r25, sar

        ldw        PCOQFRONT_OFFSET(arg0), %r25
        mtctl      %r25, pcoq

        ldw        PCOQBACK_OFFSET(arg0), %r25
        mtctl      %r25, pcoq

/*
 * Load r25 with interrupts off
 */
        ldw         R25_OFFSET(arg0),%r25
/*
 * Must load r26 (arg0) last
 */
        ldw         R26_OFFSET(arg0),%r26

isr_exit:
        rfi
        .EXIT
        .PROCEND

/*
 *  This section is used to context switch floating point registers.
 *  Ref:  6-35 of Architecture 1.1
 *
 *  NOTE:    since integer multiply uses the floating point unit,
 *           we have to save/restore fp on every trap.  We cannot
 *           just try to keep track of fp usage.
 */

	.align 32
	.EXPORT _CPU_Save_float_context,ENTRY,PRIV_LEV=0
_CPU_Save_float_context:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY
        fstds,ma    %fr0,8(%arg0)
        fstds,ma    %fr1,8(%arg0)
        fstds,ma    %fr2,8(%arg0)
        fstds,ma    %fr3,8(%arg0)
        fstds,ma    %fr4,8(%arg0)
        fstds,ma    %fr5,8(%arg0)
        fstds,ma    %fr6,8(%arg0)
        fstds,ma    %fr7,8(%arg0)
        fstds,ma    %fr8,8(%arg0)
        fstds,ma    %fr9,8(%arg0)
        fstds,ma    %fr10,8(%arg0)
        fstds,ma    %fr11,8(%arg0)
        fstds,ma    %fr12,8(%arg0)
        fstds,ma    %fr13,8(%arg0)
        fstds,ma    %fr14,8(%arg0)
        fstds,ma    %fr15,8(%arg0)
        fstds,ma    %fr16,8(%arg0)
        fstds,ma    %fr17,8(%arg0)
        fstds,ma    %fr18,8(%arg0)
        fstds,ma    %fr19,8(%arg0)
        fstds,ma    %fr20,8(%arg0)
        fstds,ma    %fr21,8(%arg0)
        fstds,ma    %fr22,8(%arg0)
        fstds,ma    %fr23,8(%arg0)
        fstds,ma    %fr24,8(%arg0)
        fstds,ma    %fr25,8(%arg0)
        fstds,ma    %fr26,8(%arg0)
        fstds,ma    %fr27,8(%arg0)
        fstds,ma    %fr28,8(%arg0)
        fstds,ma    %fr29,8(%arg0)
        fstds,ma    %fr30,8(%arg0)
        fstds       %fr31,0(%arg0)
        bv          0(%r2)
        addi        -(31*8), %arg0, %arg0        ; restore arg0 just for fun
	.EXIT
	.PROCEND

	.align 32
	.EXPORT _CPU_Restore_float_context,ENTRY,PRIV_LEV=0
_CPU_Restore_float_context:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY
        addi        (31*8), %arg0, %arg0         ; point at last double
        fldds       0(%arg0),%fr31
        fldds,mb    -8(%arg0),%fr30
        fldds,mb    -8(%arg0),%fr29
        fldds,mb    -8(%arg0),%fr28
        fldds,mb    -8(%arg0),%fr27
        fldds,mb    -8(%arg0),%fr26
        fldds,mb    -8(%arg0),%fr25
        fldds,mb    -8(%arg0),%fr24
        fldds,mb    -8(%arg0),%fr23
        fldds,mb    -8(%arg0),%fr22
        fldds,mb    -8(%arg0),%fr21
        fldds,mb    -8(%arg0),%fr20
        fldds,mb    -8(%arg0),%fr19
        fldds,mb    -8(%arg0),%fr18
        fldds,mb    -8(%arg0),%fr17
        fldds,mb    -8(%arg0),%fr16
        fldds,mb    -8(%arg0),%fr15
        fldds,mb    -8(%arg0),%fr14
        fldds,mb    -8(%arg0),%fr13
        fldds,mb    -8(%arg0),%fr12
        fldds,mb    -8(%arg0),%fr11
        fldds,mb    -8(%arg0),%fr10
        fldds,mb    -8(%arg0),%fr9
        fldds,mb    -8(%arg0),%fr8
        fldds,mb    -8(%arg0),%fr7
        fldds,mb    -8(%arg0),%fr6
        fldds,mb    -8(%arg0),%fr5
        fldds,mb    -8(%arg0),%fr4
        fldds,mb    -8(%arg0),%fr3
        fldds,mb    -8(%arg0),%fr2
        fldds,mb    -8(%arg0),%fr1
        bv          0(%r2)
        fldds,mb    -8(%arg0),%fr0
	.EXIT
	.PROCEND

/*
 * These 2 small routines are unused right now.
 * Normally we just go thru _CPU_Save_float_context (and Restore)
 *
 * Here we just deref the ptr and jump up, letting _CPU_Save_float_context
 *  do the return for us.
 */

	.EXPORT _CPU_Context_save_fp,ENTRY,PRIV_LEV=0
_CPU_Context_save_fp:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY
        bl          _CPU_Save_float_context, %r0
        ldw         0(%arg0), %arg0
	.EXIT
	.PROCEND

	.EXPORT _CPU_Context_restore_fp,ENTRY,PRIV_LEV=0
_CPU_Context_restore_fp:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY
        bl          _CPU_Restore_float_context, %r0
        ldw         0(%arg0), %arg0
	.EXIT
	.PROCEND


/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context switch.
 */

	.align 32
	.EXPORT _CPU_Context_switch,ENTRY,PRIV_LEV=0,ARGW0=GR,ARGW1=GR
_CPU_Context_switch:
    	.PROC
	.CALLINFO FRAME=64
	.ENTRY

/*
 * Save the integer context
 */
        stw         %r1,R1_OFFSET(arg0)
        stw         %r2,R2_OFFSET(arg0)
        stw         %r3,R3_OFFSET(arg0)
        stw         %r4,R4_OFFSET(arg0)
        stw         %r5,R5_OFFSET(arg0)
        stw         %r6,R6_OFFSET(arg0)
        stw         %r7,R7_OFFSET(arg0)
        stw         %r8,R8_OFFSET(arg0)
        stw         %r9,R9_OFFSET(arg0)
        stw         %r10,R10_OFFSET(arg0)
        stw         %r11,R11_OFFSET(arg0)
        stw         %r12,R12_OFFSET(arg0)
        stw         %r13,R13_OFFSET(arg0)
        stw         %r14,R14_OFFSET(arg0)
        stw         %r15,R15_OFFSET(arg0)
        stw         %r16,R16_OFFSET(arg0)
        stw         %r17,R17_OFFSET(arg0)
        stw         %r18,R18_OFFSET(arg0)
        stw         %r19,R19_OFFSET(arg0)
        stw         %r20,R20_OFFSET(arg0)
        stw         %r21,R21_OFFSET(arg0)
        stw         %r22,R22_OFFSET(arg0)
        stw         %r23,R23_OFFSET(arg0)
        stw         %r24,R24_OFFSET(arg0)
        stw         %r25,R25_OFFSET(arg0)
        stw         %r26,R26_OFFSET(arg0)
        stw         %r27,R27_OFFSET(arg0)
        stw         %r28,R28_OFFSET(arg0)
        stw         %r29,R29_OFFSET(arg0)
        stw         %r30,R30_OFFSET(arg0)
        stw         %r31,R31_OFFSET(arg0)

/*
 * fill in interrupt context section
 */
        stw         %r2, PCOQFRONT_OFFSET(%arg0)
        ldo         4(%r2), %r2
        stw         %r2, PCOQBACK_OFFSET(%arg0)

/*
 * Generate a suitable IPSW by using the system default psw
 *  with the current low bits added in.
 */

        ldil        L%CPU_PSW_DEFAULT, %r2
        ldo         R%CPU_PSW_DEFAULT(%r2), %r2
        ssm         0, %arg2
        dep         %arg2, 31, 8, %r2
        stw         %r2, IPSW_OFFSET(%arg0)

/*
 * at this point, the running task context is completely saved
 * Now jump to the bottom of the interrupt handler to load the
 * heirs context
 */

        b           _CPU_Context_restore
        copy        %arg1, %arg0

        .EXIT
        .PROCEND


/*
 * Find first bit
 * NOTE:
 *   This is used (and written) only for the ready chain code and
 *   priority bit maps.
 *   Any other use constitutes fraud.
 *   Returns first bit from the least significant side.
 *   Eg:  if input is 0x8001
 *        output will indicate the '1' bit and return 0.
 *   This is counter to HPPA bit numbering which calls this
 *   bit 31.  This way simplifies the macros _CPU_Priority_Mask
 *   and _CPU_Priority_Bits_index.
 *
 *   NOTE:
 *       We just use 16 bit version
 *       does not handle zero case
 *
 *  Based on the UTAH Mach libc version of ffs.
 */

        .align 32
	.EXPORT hppa_rtems_ffs,ENTRY,PRIV_LEV=0,ARGW0=GR
hppa_rtems_ffs:
    	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY

#ifdef RETURN_ERROR_ON_ZERO
	comb,=	%arg0,%r0,ffsdone	; If arg0 is 0
	ldi	-1,%ret0		;   return -1
#endif

#if BITFIELD_SIZE == 32
	ldi	31,%ret0		; Set return to high bit
	extru,=	%arg0,31,16,%r0		; If low 16 bits are non-zero
	addi,tr	-16,%ret0,%ret0		;   subtract 16 from bitpos
	shd	%r0,%arg0,16,%arg0	; else shift right 16 bits
#else
	ldi	15,%ret0		; Set return to high bit
#endif
	extru,=	%arg0,31,8,%r0		; If low 8 bits are non-zero
	addi,tr	-8,%ret0,%ret0		;   subtract 8 from bitpos
	shd	%r0,%arg0,8,%arg0	; else shift right 8 bits
	extru,=	%arg0,31,4,%r0		; If low 4 bits are non-zero
	addi,tr	-4,%ret0,%ret0		;   subtract 4 from bitpos
	shd	%r0,%arg0,4,%arg0	; else shift right 4 bits
	extru,=	%arg0,31,2,%r0		; If low 2 bits are non-zero
	addi,tr	-2,%ret0,%ret0		;   subtract 2 from bitpos
	shd	%r0,%arg0,2,%arg0	; else shift right 2 bits
	extru,=	%arg0,31,1,%r0		; If low bit is non-zero
	addi	-1,%ret0,%ret0		;   subtract 1 from bitpos
ffsdone:
        bv,n    0(%r2)
        nop
        .EXIT
        .PROCEND
