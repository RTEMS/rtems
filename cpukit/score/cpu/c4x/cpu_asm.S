/*  cpu_asm.c  ===> cpu_asm.S or cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <asm.h>

/*
 *  _CPU_Context_save_fp_context
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  void _CPU_Context_save_fp(
 *    void **fp_context_ptr
 *
 *  C4x Specific Information:
 *
 *  There is no distiniction between FP and integer context in this port.
 */

/*
 *  _CPU_Context_restore_fp_context
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  void _CPU_Context_restore_fp(
 *   void **fp_context_ptr
 *  )
 *
 *  C4x Specific Information:
 *
 *  There is no distiniction between FP and integer context in this port.
 */


/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  void _CPU_Context_switch(
 *    Context_Control  *run,
 *    Context_Control  *heir
 *  )
 *
 *  TMS320C3x General-Purpose Applications User's Guide, section 2.4
 *  (p 2-11 and following), Context Switching in Interrupts and 
 *  Subroutines states that "If the program is in a subroutine, it
 *  must preserve the dedicated C registers as follows:"
 *
 *    Save as Integers                 Save as Floating-Point
 *    ================                 ======================
 *      R4   R8                           R6       R7
 *      AR4  AR5
 *      AR6  AR7
 *      FP   DP (small model only)
 *      SP
 */

	.global	SYM(_CPU_Context_switch)
SYM(_CPU_Context_switch):
	.if .REGPARM == 0
	ldi     sp, ar0
	ldi     *ar0, ar2	; get the location of running context
	.endif
	sti	st,*ar2++	; store status word
	sti	ar3,*ar2++	; store ar3
	sti	ar4,*ar2++	; store ar4
	sti	ar5,*ar2++	; store ar5
	sti	ar6,*ar2++	; store ar6
	sti	ar7,*ar2++	; store ar7
	sti	r4,*ar2++	; store integer portion of r4
	sti	r5,*ar2++	; store integer portion of r5
	stf	r6,*ar2++	; store float portion of r6
	stf	r7,*ar2++	; store float portion of r7
	.if .TMS320C40
	sti	r8,*ar2++	; store integer portion of r8
	.endif
	sti	sp,*ar2++	; store sp

	; end of save

	.if .REGPARM == 0
	ldi     *-ar0(2), ar2	; get the location of heir context
	.else
	ldi	r2,ar2
	.endif
_local_restore:
	ldi	*ar2++,ar0	; load status word into register
	ldi	*ar2++,ar3	; load ar3
	ldi	*ar2++,ar4	; load ar4
	ldi	*ar2++,ar5	; load ar5
	ldi	*ar2++,ar6	; load ar6
	ldi	*ar2++,ar7	; load ar7
	ldi	*ar2++,r4	; load integer portion of r4
	ldi	*ar2++,r5	; load integer portion of r5
	ldf	*ar2++,r6	; load float portion of r6
	ldf	*ar2++,r7	; load float portion of r7
	.if .TMS320C40
	ldi	*ar2++,r8	; load integer portion of r8
	.endif
	ldi	*ar2++,sp	; load sp
	ldi	ar0,st		; restore status word and interrupts
	rets

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *  
 *  void _CPU_Context_restore(
 *    Context_Control *new_context
 *  )
 */

	.global	SYM(_CPU_Context_restore)
SYM(_CPU_Context_restore):
	.if .REGPARM == 0
	ldi     sp, ar0
	ldi     *ar0, ar2	; get the location of context to restore
	.endif
	br	_local_restore

/*  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  void _ISR_Handler()
 */

  /*
   *  At entry to "common" _ISR_Handler, the vector number must be
   *  available.  On some CPUs the hardware puts either the vector
   *  number or the offset into the vector table for this ISR in a
   *  known place.  If the hardware does not give us this information,
   *  then the assembly portion of RTEMS for this port will contain
   *  a set of distinct interrupt entry points which somehow place
   *  the vector number in a known place (which is safe if another
   *  interrupt nests this one) and branches to _ISR_Handler.
   */

  /*
   *  save some or all context on stack
   *  may need to save some special interrupt information for exit
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if ( _ISR_Nest_level == 0 )
   *      switch to software interrupt stack
   *  #endif
   *
   *  _ISR_Nest_level++;
   *
   *  _Thread_Dispatch_disable_level++;
   *
   *  (*_ISR_Vector_table[ vector ])( vector );
   *
   *  --_ISR_Nest_level;
   *
   *  if ( _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    restore stack
   *  #endif
   *  
   *  if ( !_Context_Switch_necessary )
   *    goto the label "exit interrupt (simple case)"
   *  
   *  if ( !_ISR_Signals_to_thread_executing )
   *    _ISR_Signals_to_thread_executing = FALSE;
   *    goto the label "exit interrupt (simple case)"
   *
   *  call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   *
   *  prepare to get out of interrupt
   *  return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):
   *  prepare to get out of interrupt
   *  return from interrupt
   */

	.global	SYM(_ISR_Handler_save_registers)
SYM(_ISR_Handler_save_registers):
	; no push st because it is already pushed 
	; no push ar2 because it is already pushed and vector number loaded
	push	ar0
	push	ar1
	push	dp
	push	ir0
	push	ir1
	push	rs
	push	re
	push	rc
	push	bk

	push	r0
	pushf	r0
	push	r1
	pushf	r1
	push	r2
	pushf	r2
	push	r3
	pushf	r3
	; no push r4 because other part of register is in basic context
	push	r4
	pushf	r4
	; no push r5 because other part of register is in basic context
	push	r5
	pushf	r5
	push	r6
	pushf	r6
	; no pushf r6 because other part of register is in basic context
	push	r7
	pushf	r7
	; no pushf r7 because other part of register is in basic context
	.if .TMS320C40
	push	r8
	; no pushf r8 because other part of register is in basic context
	push	r9
	pushf	r9
	push	r10
	pushf	r10
	push	r11
	pushf	r11
	.endif

	ldi	sp,r2
	call	SYM(__ISR_Handler)

	.if .TMS320C40
	popf	r11
	pop	r11
	popf	r10
	pop	r10
	popf	r9
	pop	r9
	; no popf r8 because other part of register is in basic context
	pop	r8
	.endif
	; no popf r7 because other part of register is in basic context
	popf	r7
	pop	r7
	; no popf r6 because other part of register is in basic context
	popf	r6
	pop	r6
	; no popf r5 because other part of register is in basic context
	popf	r5
	pop	r5
	; no pop r4 because other part of register is in basic context
	popf	r4
	pop	r4
	popf	r3 
	pop	r3
	popf	r2
	pop	r2
	popf	r1
	pop	r1
	popf	r0
	pop	r0
   
	pop	bk
	pop	rc
	pop	re
	pop	rs
	pop	ir1
	pop	ir0
	pop	dp
	pop	ar1
	pop	ar0
	pop	ar2	; because the vector numbers goes here
	pop	st
	reti

/*
 *  Prologues so we can know the vector number.  Generated by this script:
 *
 *    i=0
 *    while test $i -lt 64 
 *    do
 *    
 *       printf "\t.global\tSYM(rtems_irq_prologue_%X)\n" $i
 *       printf "SYM(rtems_irq_prologue_%X):\n" $i
 *       printf "\tpush\tst\n"
 *       printf "\tpush\tar2\n"
 *       printf "\tldi\t0x%x,ar2\n" $i
 *       printf "\tbr\tSYM(_ISR_Handler_save_registers)\n"
 *       printf "\n"
 *       i=`expr $i + 1`
 *    
 *    done
 */

	.global	SYM(rtems_irq_prologue_0)
SYM(rtems_irq_prologue_0):
	push	st
	push	ar2
	ldi	0x0,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1)
SYM(rtems_irq_prologue_1):
	push	st
	push	ar2
	ldi	0x1,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2)
SYM(rtems_irq_prologue_2):
	push	st
	push	ar2
	ldi	0x2,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3)
SYM(rtems_irq_prologue_3):
	push	st
	push	ar2
	ldi	0x3,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_4)
SYM(rtems_irq_prologue_4):
	push	st
	push	ar2
	ldi	0x4,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_5)
SYM(rtems_irq_prologue_5):
	push	st
	push	ar2
	ldi	0x5,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_6)
SYM(rtems_irq_prologue_6):
	push	st
	push	ar2
	ldi	0x6,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_7)
SYM(rtems_irq_prologue_7):
	push	st
	push	ar2
	ldi	0x7,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_8)
SYM(rtems_irq_prologue_8):
	push	st
	push	ar2
	ldi	0x8,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_9)
SYM(rtems_irq_prologue_9):
	push	st
	push	ar2
	ldi	0x9,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_A)
SYM(rtems_irq_prologue_A):
	push	st
	push	ar2
	ldi	0xa,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_B)
SYM(rtems_irq_prologue_B):
	push	st
	push	ar2
	ldi	0xb,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_C)
SYM(rtems_irq_prologue_C):
	push	st
	push	ar2
	ldi	0xc,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_D)
SYM(rtems_irq_prologue_D):
	push	st
	push	ar2
	ldi	0xd,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_E)
SYM(rtems_irq_prologue_E):
	push	st
	push	ar2
	ldi	0xe,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_F)
SYM(rtems_irq_prologue_F):
	push	st
	push	ar2
	ldi	0xf,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_10)
SYM(rtems_irq_prologue_10):
	push	st
	push	ar2
	ldi	0x10,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_11)
SYM(rtems_irq_prologue_11):
	push	st
	push	ar2
	ldi	0x11,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_12)
SYM(rtems_irq_prologue_12):
	push	st
	push	ar2
	ldi	0x12,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_13)
SYM(rtems_irq_prologue_13):
	push	st
	push	ar2
	ldi	0x13,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_14)
SYM(rtems_irq_prologue_14):
	push	st
	push	ar2
	ldi	0x14,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_15)
SYM(rtems_irq_prologue_15):
	push	st
	push	ar2
	ldi	0x15,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_16)
SYM(rtems_irq_prologue_16):
	push	st
	push	ar2
	ldi	0x16,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_17)
SYM(rtems_irq_prologue_17):
	push	st
	push	ar2
	ldi	0x17,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_18)
SYM(rtems_irq_prologue_18):
	push	st
	push	ar2
	ldi	0x18,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_19)
SYM(rtems_irq_prologue_19):
	push	st
	push	ar2
	ldi	0x19,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1A)
SYM(rtems_irq_prologue_1A):
	push	st
	push	ar2
	ldi	0x1a,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1B)
SYM(rtems_irq_prologue_1B):
	push	st
	push	ar2
	ldi	0x1b,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1C)
SYM(rtems_irq_prologue_1C):
	push	st
	push	ar2
	ldi	0x1c,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1D)
SYM(rtems_irq_prologue_1D):
	push	st
	push	ar2
	ldi	0x1d,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1E)
SYM(rtems_irq_prologue_1E):
	push	st
	push	ar2
	ldi	0x1e,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_1F)
SYM(rtems_irq_prologue_1F):
	push	st
	push	ar2
	ldi	0x1f,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_20)
SYM(rtems_irq_prologue_20):
	push	st
	push	ar2
	ldi	0x20,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_21)
SYM(rtems_irq_prologue_21):
	push	st
	push	ar2
	ldi	0x21,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_22)
SYM(rtems_irq_prologue_22):
	push	st
	push	ar2
	ldi	0x22,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_23)
SYM(rtems_irq_prologue_23):
	push	st
	push	ar2
	ldi	0x23,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_24)
SYM(rtems_irq_prologue_24):
	push	st
	push	ar2
	ldi	0x24,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_25)
SYM(rtems_irq_prologue_25):
	push	st
	push	ar2
	ldi	0x25,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_26)
SYM(rtems_irq_prologue_26):
	push	st
	push	ar2
	ldi	0x26,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_27)
SYM(rtems_irq_prologue_27):
	push	st
	push	ar2
	ldi	0x27,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_28)
SYM(rtems_irq_prologue_28):
	push	st
	push	ar2
	ldi	0x28,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_29)
SYM(rtems_irq_prologue_29):
	push	st
	push	ar2
	ldi	0x29,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2A)
SYM(rtems_irq_prologue_2A):
	push	st
	push	ar2
	ldi	0x2a,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2B)
SYM(rtems_irq_prologue_2B):
	push	st
	push	ar2
	ldi	0x2b,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2C)
SYM(rtems_irq_prologue_2C):
	push	st
	push	ar2
	ldi	0x2c,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2D)
SYM(rtems_irq_prologue_2D):
	push	st
	push	ar2
	ldi	0x2d,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2E)
SYM(rtems_irq_prologue_2E):
	push	st
	push	ar2
	ldi	0x2e,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_2F)
SYM(rtems_irq_prologue_2F):
	push	st
	push	ar2
	ldi	0x2f,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_30)
SYM(rtems_irq_prologue_30):
	push	st
	push	ar2
	ldi	0x30,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_31)
SYM(rtems_irq_prologue_31):
	push	st
	push	ar2
	ldi	0x31,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_32)
SYM(rtems_irq_prologue_32):
	push	st
	push	ar2
	ldi	0x32,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_33)
SYM(rtems_irq_prologue_33):
	push	st
	push	ar2
	ldi	0x33,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_34)
SYM(rtems_irq_prologue_34):
	push	st
	push	ar2
	ldi	0x34,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_35)
SYM(rtems_irq_prologue_35):
	push	st
	push	ar2
	ldi	0x35,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_36)
SYM(rtems_irq_prologue_36):
	push	st
	push	ar2
	ldi	0x36,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_37)
SYM(rtems_irq_prologue_37):
	push	st
	push	ar2
	ldi	0x37,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_38)
SYM(rtems_irq_prologue_38):
	push	st
	push	ar2
	ldi	0x38,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_39)
SYM(rtems_irq_prologue_39):
	push	st
	push	ar2
	ldi	0x39,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3A)
SYM(rtems_irq_prologue_3A):
	push	st
	push	ar2
	ldi	0x3a,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3B)
SYM(rtems_irq_prologue_3B):
	push	st
	push	ar2
	ldi	0x3b,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3C)
SYM(rtems_irq_prologue_3C):
	push	st
	push	ar2
	ldi	0x3c,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3D)
SYM(rtems_irq_prologue_3D):
	push	st
	push	ar2
	ldi	0x3d,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3E)
SYM(rtems_irq_prologue_3E):
	push	st
	push	ar2
	ldi	0x3e,ar2
	br	SYM(_ISR_Handler_save_registers)

	.global	SYM(rtems_irq_prologue_3F)
SYM(rtems_irq_prologue_3F):
	push	st
	push	ar2
	ldi	0x3f,ar2
	br	SYM(_ISR_Handler_save_registers)

