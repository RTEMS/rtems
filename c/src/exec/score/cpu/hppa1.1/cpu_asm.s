# 	@(#)cpu_asm.S	1.6 - 95/05/16
# 	
# 
# TODO: 
#       Context_switch needs to only save callee save registers
#       I think this means can skip:    r1, r2, r19-29, r31
#       Ref:     p 3-2 of Procedure Calling Conventions Manual
#       This should be #ifndef DEBUG so that debugger has
#       accurate visibility into all registers
#
#  This file contains the assembly code for the HPPA implementation
#  of RTEMS.
#
#  COPYRIGHT (c) 1994,95 by Division Incorporated
#
#  To anyone who acknowledges that this file is provided "AS IS"
#  without any express or implied warranty:
#      permission to use, copy, modify, and distribute this file
#      for any purpose is hereby granted without fee, provided that
#      the above copyright notice and this notice appears in all
#      copies, and that the name of Division Incorporated not be
#      used in advertising or publicity pertaining to distribution
#      of the software without specific, written prior permission.
#      Division Incorporated makes no representations about the
#      suitability of this software for any purpose.
#
#  cpu_asm.S,v 1.2 1995/05/09 20:11:37 joel Exp
#

#include <rtems/hppa.h>
#include <rtems/cpu_asm.h>
#include <rtems/cpu.h>

#include <offsets.h>

	.SPACE $PRIVATE$
	.SUBSPA $DATA$,QUAD=1,ALIGN=8,ACCESS=31
	.SUBSPA $BSS$,QUAD=1,ALIGN=8,ACCESS=31,ZERO,SORT=82
	.SPACE $TEXT$
	.SUBSPA $LIT$,QUAD=0,ALIGN=8,ACCESS=44
	.SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=44,CODE_ONLY
	.SPACE $TEXT$
	.SUBSPA $CODE$

#
# Special register usage for context switch and interrupts
# Stay away from %cr28 which is used for TLB misses on 72000
#

isr_arg0           .reg    %cr24
isr_r9             .reg    %cr25

#
# Interrupt stack frame looks like this
#
#  offset                                   item
# -----------------------------------------------------------------
#   INTEGER_CONTEXT_OFFSET             Context_Control
#   FP_CONTEXT_OFFSET                  Context_Control_fp
#
# It is padded out to a multiple of 64
#


#  PAGE^L
#  void __Generic_ISR_Handler()
#
#  This routine provides the RTEMS interrupt management.
#
#  NOTE:
#    Upon entry, the stack will contain a stack frame back to the
#    interrupted task.  If dispatching is enabled, this is the
#    outer most interrupt, (and a context switch is necessary or
#    the current task has signals), then set up the stack to
#    transfer control to the interrupt dispatcher.
#
#
#   We jump here from the interrupt vector.
#   The hardware has done some stuff for us:
#       PSW saved in IPSW
#       PSW set to 0
#       PSW[E] set to default (0)
#       PSW[M] set to 1 iff this is HPMC
#
#       IIA queue is frozen (since PSW[Q] is now 0)
#       privilege level promoted to 0
#       IIR, ISR, IOR potentially updated if PSW[Q] was 1 at trap
#       registers GR  1,8,9,16,17,24,25 copied to shadow regs
#                 SHR 0 1 2  3  4  5  6
#
#   Our vector stub did the following
#       placed vector number is in r1
#
#        stub
#            r1 <- vector number
#            save ipsw under rock
#            ipsw = ipsw & ~1    --  disable ints
#            save qregs under rock
#            qra = _Generic_ISR_handler
#            rfi
#
################################################

#  Distinct Interrupt Entry Points
#
#  The following macro and the 32 instantiations of the macro
#  are necessary to determine which interrupt vector occurred.
#  The following macro allows a unique entry point to be defined
#  for each vector.
#
# r9 was loaded with the vector before branching here
#   scratch registers available:   gr1, gr8, gr9, gr16, gr17, gr24
#
# NOTE:
#  .align 32   doesn not seem to work in the continuation below
#  so just have to count 8 instructions
#
# NOTE:
#    this whole scheme needs to be rethought for TLB traps which
#    have requirements about what tlb faults they can incur.
#    ref:  TLB Operation Requirements in 1.1 arch book

#define THANDLER(vector) \
        mtctl   %r9,  isr_r9 !         \
        b       _Generic_ISR_Handler!        \
        ldi     vector, %r9!       \
        nop ! \
        nop ! \
        nop ! \
        nop ! \
        nop

        .align 4096
	.EXPORT IVA_Table,ENTRY,PRIV_LEV=0
IVA_Table:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY

        THANDLER(0)     	/* unused */

        THANDLER(HPPA_INTERRUPT_HIGH_PRIORITY_MACHINE_CHECK)

        THANDLER(HPPA_INTERRUPT_POWER_FAIL)

        THANDLER(HPPA_INTERRUPT_RECOVERY_COUNTER)

        THANDLER(HPPA_INTERRUPT_EXTERNAL_INTERRUPT)

        THANDLER(HPPA_INTERRUPT_LOW_PRIORITY_MACHINE_CHECK)

        THANDLER(HPPA_INTERRUPT_INSTRUCTION_TLB_MISS)

        THANDLER(HPPA_INTERRUPT_INSTRUCTION_MEMORY_PROTECTION)

        THANDLER(HPPA_INTERRUPT_ILLEGAL_INSTRUCTION)

        THANDLER(HPPA_INTERRUPT_BREAK_INSTRUCTION)

        THANDLER(HPPA_INTERRUPT_PRIVILEGED_OPERATION)

        THANDLER(HPPA_INTERRUPT_PRIVILEGED_REGISTER)

        THANDLER(HPPA_INTERRUPT_OVERFLOW)

        THANDLER(HPPA_INTERRUPT_CONDITIONAL)

        THANDLER(HPPA_INTERRUPT_ASSIST_EXCEPTION)

        THANDLER(HPPA_INTERRUPT_DATA_TLB_MISS)

        THANDLER(HPPA_INTERRUPT_NON_ACCESS_INSTRUCTION_TLB_MISS)

        THANDLER(HPPA_INTERRUPT_NON_ACCESS_DATA_TLB_MISS)

        THANDLER(HPPA_INTERRUPT_DATA_MEMORY_PROTECTION)

        THANDLER(HPPA_INTERRUPT_DATA_MEMORY_BREAK)

        THANDLER(HPPA_INTERRUPT_TLB_DIRTY_BIT)

        THANDLER(HPPA_INTERRUPT_PAGE_REFERENCE)

        THANDLER(HPPA_INTERRUPT_ASSIST_EMULATION)

        THANDLER(HPPA_INTERRUPT_HIGHER_PRIVILEGE_TRANSFER)

        THANDLER(HPPA_INTERRUPT_LOWER_PRIVILEGE_TRANSFER)

        THANDLER(HPPA_INTERRUPT_TAKEN_BRANCH)

        THANDLER(HPPA_INTERRUPT_DATA_MEMORY_ACCESS_RIGHTS)

        THANDLER(HPPA_INTERRUPT_DATA_MEMORY_PROTECTION_ID)

        THANDLER(HPPA_INTERRUPT_UNALIGNED_DATA_REFERENCE)

        THANDLER(HPPA_INTERRUPT_PERFORMANCE_MONITOR)

        THANDLER(HPPA_INTERRUPT_INSTRUCTION_DEBUG)

        THANDLER(HPPA_INTERRUPT_DATA_DEBUG)

        .EXIT
        .PROCEND

	.EXPORT _Generic_ISR_Handler,ENTRY,PRIV_LEV=0
_Generic_ISR_Handler:
	.PROC
	.CALLINFO FRAME=0,NO_CALLS
	.ENTRY

# Turn on the D bit in psw so we can start saving stuff on stack
# (interrupt context pieces that need to be saved before the RFI)

        ssm       HPPA_PSW_D, %r0
        mtctl     arg0, isr_arg0

# save interrupt state
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

# Prepare to re-enter virtual mode
# We need Q in case the interrupt handler enables interrupts
#

        ldil      L%CPU_PSW_DEFAULT, arg0
        ldo       R%CPU_PSW_DEFAULT(arg0), arg0
        mtctl     arg0, ipsw

# Now jump to "rest_of_isr_handler" with the rfi
# We are assuming the space queues are all correct already

	ldil 	  L%rest_of_isr_handler, arg0
	ldo	  R%rest_of_isr_handler(arg0), arg0
	mtctl	  arg0, pcoq
	ldo	  4(arg0), arg0
	mtctl	  arg0, pcoq

        rfi
	nop

# At this point we are back in virtual mode and all our
#  normal addressing is once again ok.

rest_of_isr_handler:

#
# Build an interrupt frame to hold the contexts we will need.
# We have already saved the interrupt items on the stack

# At this point the following registers are damaged wrt the interrupt
#  reg    current value        saved value
# ------------------------------------------------
#  arg0   scratch               isr_arg0  (ctl)
#  r9     vector number         isr_r9    (ctl)
#
# Point to beginning of integer context and
# save the integer context
        stw         %r1,R1_OFFSET(sp)
        stw         %r2,R2_OFFSET(sp)
        stw         %r3,R3_OFFSET(sp)
        stw         %r4,R4_OFFSET(sp)
        stw         %r5,R5_OFFSET(sp)
        stw         %r6,R6_OFFSET(sp)
        stw         %r7,R7_OFFSET(sp)
        stw         %r8,R8_OFFSET(sp)
        stw         %r9,R9_OFFSET(sp)
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
        stw         %r26,R26_OFFSET(sp)
        stw         %r27,R27_OFFSET(sp)
        stw         %r28,R28_OFFSET(sp)
        stw         %r29,R29_OFFSET(sp)
        stw         %r30,R30_OFFSET(sp)
        stw         %r31,R31_OFFSET(sp)

# Now most registers are available since they have been saved
#
# The following items are currently wrong in the integer context
#  reg    current value        saved value
# ------------------------------------------------
#  arg0   scratch               isr_arg0  (ctl)
#  r9     vector number         isr_r9    (ctl)
#
# Fix them

         mfctl      isr_arg0,%r3
         stw        %r3,ARG0_OFFSET(sp)

         mfctl      isr_r9,%r3
         stw        %r3,R9_OFFSET(sp)

#
# At this point we are done with isr_arg0, and isr_r9 control registers
#


# Point to beginning of float context and
# save the floating point context -- doing whatever patches are necessary
        .call ARGW0=GR
        bl          _CPU_Save_float_context,%r2
        ldo         FP_CONTEXT_OFFSET(sp),arg0

# save the ptr to interrupt frame as an argument for the interrupt handler
        copy        sp, arg1

# Advance the frame to point beyond all interrupt contexts (integer & float)
# this also includes the pad to align to 64byte stack boundary
        ldo         CPU_INTERRUPT_FRAME_SIZE(sp), sp

#    r3  -- &_ISR_Nest_level
#    r5  -- value _ISR_Nest_level
#    r4  -- &_Thread_Dispatch_disable_level
#    r6  -- value _Thread_Dispatch_disable_level
#    r9  -- vector number

 	.import   _ISR_Nest_level,data
	ldil	  L%_ISR_Nest_level,%r3
	ldo	  R%_ISR_Nest_level(%r3),%r3
	ldw	  0(%r3),%r5

	.import   _Thread_Dispatch_disable_level,data
	ldil	  L%_Thread_Dispatch_disable_level,%r4
	ldo	  R%_Thread_Dispatch_disable_level(%r4),%r4
	ldw	  0(%r4),%r6

# increment interrupt nest level counter.  If outermost interrupt
# switch the stack and squirrel away the previous sp.
        addi      1,%r5,%r5
        stw       %r5, 0(%r3)

# compute and save new stack (with frame)
# just in case we are nested -- simpler this way
        comibf,=  1,%r5,stack_done
        ldo       128(sp),%r7

#
# Switch to interrupt stack allocated by the interrupt manager (intr.c)
#
        .import   _CPU_Interrupt_stack_low,data
	ldil	  L%_CPU_Interrupt_stack_low,%r7
	ldw	  R%_CPU_Interrupt_stack_low(%r7),%r7
        ldo       128(%r7),%r7

stack_done:
# save our current stack pointer where the "old sp" is supposed to be
        stw       sp, -4(%r7)
# and switch stacks (or advance old stack in nested case)
        copy      %r7, sp

# increment the dispatch disable level counter.
        addi      1,%r6,%r6
        stw       %r6, 0(%r4)

# load address of user handler
        .import   _ISR_Vector_table,data
	ldil	  L%_ISR_Vector_table,%r8
	ldo	  R%_ISR_Vector_table(%r8),%r8
        ldwx,s    %r9(%r8),%r8

# invoke user interrupt handler
# Interrupts are currently disabled, as per RTEMS convention
# The handler has the option of re-enabling interrupts
# NOTE:  can not use 'bl' since it uses "pc-relative" addressing
#    and we are using a hard coded address from a table
#  So... we fudge r2 ourselves (ala dynacall)
#
        copy      %r9, %r26
        .call  ARGW0=GR, ARGW1=GR
        blr       %r0, rp
        bv,n      0(%r8)

post_user_interrupt_handler:

# Back from user handler(s)
# Disable external interrupts (since the interrupt handler could
# have turned them on) and return to the interrupted task stack (assuming
# (_ISR_Nest_level == 0)

        rsm        HPPA_PSW_I, %r0
        ldw        -4(sp), sp

#    r3  -- &_ISR_Nest_level
#    r5  -- value _ISR_Nest_level
#    r4  -- &_Thread_Dispatch_disable_level
#    r6  -- value _Thread_Dispatch_disable_level

 	.import   _ISR_Nest_level,data
	ldil	  L%_ISR_Nest_level,%r3
	ldo	  R%_ISR_Nest_level(%r3),%r3
	ldw	  0(%r3),%r5

	.import   _Thread_Dispatch_disable_level,data
	ldil	  L%_Thread_Dispatch_disable_level,%r4
	ldo	  R%_Thread_Dispatch_disable_level(%r4),%r4
	ldw	  0(%r4), %r6

# decrement isr nest level
	addi      -1, %r5, %r5
        stw       %r5, 0(%r3)

# decrement dispatch disable level counter and, if not 0, go on
        addi       -1,%r6,%r6
        comibf,=   0,%r6,isr_restore
        stw        %r6, 0(%r4)

# check whether or not a context switch is necessary
	.import    _Context_Switch_necessary,data
	ldil	   L%_Context_Switch_necessary,%r8
	ldw	   R%_Context_Switch_necessary(%r8),%r8
	comibf,=,n 0,%r8,ISR_dispatch

# check whether or not a context switch is necessary because an ISR
#    sent signals to the interrupted task
	.import    _ISR_Signals_to_thread_executing,data
	ldil	   L%_ISR_Signals_to_thread_executing,%r8
	ldw	   R%_ISR_Signals_to_thread_executing(%r8),%r8
	comibt,=,n 0,%r8,isr_restore

# OK, something happened while in ISR and we need to switch to a task
# other than the one which was interrupted or the
#    ISR_Signals_to_thread_executing case
# We also turn on interrupts, since the interrupted task had them
#   on (obviously :-) and Thread_Dispatch is happy to leave ints on.
#

ISR_dispatch:
        ssm        HPPA_PSW_I, %r0

        .import    _Thread_Dispatch,code
        .call
        bl         _Thread_Dispatch,%r2
        ldo        128(sp),sp

        ldo        -128(sp),sp

        rsm        HPPA_PSW_I, %r0

isr_restore:

# Get a pointer to beginning of our stack frame
        ldo        -CPU_INTERRUPT_FRAME_SIZE(sp), %arg1

# restore float
        .call ARGW0=GR
        bl         _CPU_Restore_float_context,%r2
        ldo        FP_CONTEXT_OFFSET(%arg1), arg0

        copy       %arg1, %arg0

#   ********** FALL THRU **********

# Jump here from bottom of Context_Switch
# Also called directly by _CPU_Context_Restart_self via _Thread_Restart_self
# restore interrupt state
#

	.EXPORT _CPU_Context_restore
_CPU_Context_restore:

# Turn off Q & I so we can write pcoq
        rsm        HPPA_PSW_Q + HPPA_PSW_I, %r0

        ldw        IPSW_OFFSET(arg0), %r8
        mtctl      %r8, ipsw

        ldw        SAR_OFFSET(arg0), %r9
        mtctl      %r9, sar

        ldw        PCOQFRONT_OFFSET(arg0), %r10
        mtctl      %r10, pcoq

        ldw        PCOQBACK_OFFSET(arg0), %r11
        mtctl      %r11, pcoq

#
# restore integer state
#
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
        ldw         R25_OFFSET(arg0),%r25
# skipping r26 (aka arg0) until we are done with it
        ldw         R27_OFFSET(arg0),%r27
        ldw         R28_OFFSET(arg0),%r28
        ldw         R29_OFFSET(arg0),%r29
        ldw         R30_OFFSET(arg0),%r30
        ldw         R31_OFFSET(arg0),%r31

# Must load r26 last since it is arg0
        ldw         R26_OFFSET(arg0),%r26

isr_exit:
        rfi
        .EXIT
        .PROCEND

#
#  This section is used to context switch floating point registers.
#  Ref:  6-35 of Architecture 1.1
#
#  NOTE:    since integer multiply uses the floating point unit,
#           we have to save/restore fp on every trap.  We cannot
#           just try to keep track of fp usage.

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

#
# These 2 small routines are unused right now.
# Normally we just go thru _CPU_Save_float_context (and Restore)
#
# Here we just deref the ptr and jump up, letting _CPU_Save_float_context
#  do the return for us.
#
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


#  void _CPU_Context_switch( run_context, heir_context )
#
#  This routine performs a normal non-FP context switch.
#

	.align 32
	.EXPORT _CPU_Context_switch,ENTRY,PRIV_LEV=0,ARGW0=GR,ARGW1=GR
_CPU_Context_switch:
    	.PROC
	.CALLINFO FRAME=64
	.ENTRY

# Save the integer context
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

# fill in interrupt context section
        stw         %r2, PCOQFRONT_OFFSET(%arg0)
        ldo         4(%r2), %r2
        stw         %r2, PCOQBACK_OFFSET(%arg0)

# Generate a suitable IPSW by using the system default psw
#  with the current low bits added in.

        ldil        L%CPU_PSW_DEFAULT, %r2
        ldo         R%CPU_PSW_DEFAULT(%r2), %r2
        ssm         0, %arg2
        dep         %arg2, 31, 8, %r2
        stw         %r2, IPSW_OFFSET(%arg0)

# at this point, the running task context is completely saved
# Now jump to the bottom of the interrupt handler to load the
# heirs context

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
