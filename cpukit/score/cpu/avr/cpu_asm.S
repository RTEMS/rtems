/*  cpu_asm.c  ===> cpu_asm.S or cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" cpu_asm file.  An
 *  implementation in assembly should include "cpu_asm.h>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <avr/sfr_defs.h>
#include <rtems/score/percpu.h>


#define jmpb_hi 	r25
#define jmpb_lo 	r24
#define val_hi 		r23
#define val_lo 		r22

#define ret_lo		r24
#define ret_hi 		r25

	PUBLIC( setjmp )

SYM( setjmp ):
	X_movw		XL, jmpb_lo
/*;save call-saved registers and frame pointer*/
	.irp		.L_regno, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,28,29	
	st		X+, r\.L_regno
	.endr
/*;get return address*/
	
	pop		ZH
	pop		ZL
/*save stack pointer (after popping)*/
	
	in 		ret_lo, 	AVR_STACK_POINTER_LO_ADDR
	st		X+, ret_lo
	
#ifdef _HAVE_AVR_STACK_POINTER_HI
	in 		ret_lo, AVR_STACK_POINTER_HI_ADDR
	st 		X+, ret_lo
#else
	st		X+, ret_lo
#endif
/*save status reg (I flag)*/
	in		ret_lo, AVR_STATUS_ADDR
	st		X+, ret_lo
/*save return addr*/
	st		X+, ZL
	st		X+, ZH
/*return zero*/
	clr 		ret_hi
	clr		ret_lo
	ijmp

	.size		_U(setjmp),.-_U(setjmp)


	.global _U(longjmp)
	.type _U(longjmp), @function

_U(longjmp):
	X_movw		XL, jmpb_lo
/*return value*/
	X_movw		ret_lo, val_lo
/*if zero, change to 1*/
	cpi		ret_lo, 1
	cpc		ret_hi, __zero_reg__
	adc		ret_lo, __zero_reg__
/*restore call-saved registers and frame pointer*/
	.irp	.L_regno, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,28,29
	ld	r\.L_regno, X+
	.endr
/*; restore stack pointer (SP value before the setjmp() call) and SREG*/
	ld	ZL, X+
	ld	ZH, X+
	ld	__tmp_reg__, X+
#if  defined (__AVR_XMEGA__) && __AVR_XMEGA__
	/* A write to SPL will automatically disable interrupts for up to 4
	   instructions or until the next I/O memory write.	*/
	out	AVR_STATUS_ADDR, __tmp_reg__
	out	AVR_STACK_POINTER_LO_ADDR, ZL
	out	AVR_STACK_POINTER_HI_ADDR, ZH
#else
# ifdef _HAVE_AVR_STACK_POINTER_HI
	/* interrupts disabled for shortest possible time (3 cycles) */
	cli
	out	AVR_STACK_POINTER_HI_ADDR, ZH
# endif
	/* Restore status register (including the interrupt enable flag).
	   Interrupts are re-enabled only after the next instruction.  */
	out	AVR_STATUS_ADDR, __tmp_reg__
	out	AVR_STACK_POINTER_LO_ADDR, ZL
#endif
  ; get return address and jump
	ld	ZL, X+
	ld	ZH, X+
#if  defined(__AVR_3_BYTE_PC__) && __AVR_3_BYTE_PC__
	ld	__tmp_reg__, X+
.L_jmp3:
	push	ZL
	push	ZH
	push	__tmp_reg__
	ret
#else
	ijmp
#endif
	.size	_U(longjmp), . - _U(longjmp)



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
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate


void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
)
{
}
*/

	PUBLIC(_CPU_Context_save_fp)

SYM(_CPU_Context_save_fp):
	ret


	




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
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate


void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
)
{
}
*/


	PUBLIC(_CPU_Context_restore_fp)

SYM(_CPU_Context_restore_fp):
	ret



/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

*/

	PUBLIC(_CPU_Context_switch)
SYM(_CPU_Context_switch):
	mov	r26, r24 	/*r26,r27 is X*/
	mov	r27, r25
	mov	r24, r22
 	mov	r25, r23
	/*save registers*/
#if 1
/*if this section is removed there is a problem.*/
/*debug section start*/
	pop	r22
	pop	r23
	push	r22
	push	r23
/*debug section end*/
#endif

	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17

	push	r28
	push	r29

	/*load sreg*/
	lds 	r23,0x5f  /*load sreg*/
	/*disable interrupts*/
	cli
	/*load stack pointer*/
	lds	r22,0x5d /*spl*/
	lds 	r21,0x5e /*sph*/
	/*save sreg and sp to context struct*/

	/*save low then high byte --- verify this delete when verified*/
	st	X+, r22
	st	X+, r21
	st 	X, r23

	PUBLIC(_CPU_Context_restore)

SYM(_CPU_Context_restore):
	mov	r26,r24         /* R26/27 are X */
	mov	r27,r25

	/*restore stack pointer*/
	ld	r25, X+
	ld	r24, X+	
	sts	0x5E,r24  /*sph*/
	sts 	0x5D ,r25 /*spl*/
	/*restore registers from stack*/


	pop	r29
	pop	r28


	pop	r17
	pop	r16
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2

	/*restore sreg*/
	ld	r25, X
	sts	0x5f,r25 /*sreg*/
	pop	r30
	pop	r31
	IJMP
	ret



/*
 *  _CPU_Context_Initialize
 *
 *  This kernel routine initializes the basic non-FP context area associated
 *  with each thread.
 *
 *  Input parameters:
 *    the_context  - pointer to the context area
 *    stack_base   - address of memory for the SPARC
 *    size         - size in bytes of the stack area
 *    new_level    - interrupt level for this context area
 *    entry_point  - the starting execution point for this this context
 *    is_fp        - TRUE if this context is associated with an FP thread
 *
 *	the_context is in r25,r24
 *	entry point is in r13,r12
 *	stack base is in r23, r22
 *	size is in r21, r20, r19,r18
 *	newleve is in r14,r15, r16, 17
 *
 *
 *  Output parameters: NONE
 */

	PUBLIC(_CPU_Context_Initialize)
SYM(_CPU_Context_Initialize):
	//save caller saved regs
	PUSH	R10
	PUSH 	R11
	PUSH	R12
	PUSH 	R13
	PUSH 	R14
	PUSH 	R15
	PUSH 	R16
	PUSH	R17
	PUSH	R28
	PUSH	R29
	//calculate new stack pointer
	ADD	R22, R18
	ADC	R23, R19
	MOV	R26, R22
	MOV	R27, R23
	//Initialize stack with entry point
	ST	-X, R13
	ST	-X, R12
	//store new stack pointer in context control
	SBIW	R26, 0X13	/*subtract 33 to account for registers*/
	MOV	R28, R24
	MOV	R29, R25
	STD	Y+1, R27	//save stack pointer high to context control
	ST	Y, R26		//save stack pointer low to context control
	//set interrupt level in new context
	LDI	R18, 0		//set sreg in new context to zero
	STD	Y+2, R18 	//interrupts not enabled		
	MOV	R18, R14
	CPI	R18, 0
	BRNE	NEW_LEVEL_ZERO
	LDI	R18, 0X80	//set sreg in new context to 0x80
	STD	Y+2, R18	//interupts enabled
NEW_LEVEL_ZERO:
	//restore caller saved regs
	POP	R29
	POP	R28
	POP	R17
	POP	R16
	POP	R15
	POP	R14
	POP	R13
	POP	R12
	POP	R11
	POP	R10
	RET
	


/*  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate


void _ISR_Handler(void)
{

*/
   /*
    *  This discussion ignores a lot of the ugly details in a real
    *  implementation such as saving enough registers/state to be
    *  able to do something real.  Keep in mind that the goal is
    *  to invoke a user's ISR handler which is written in C and
    *  uses a certain set of registers.
    *
    *  Also note that the exact order is to a large extent flexible.
    *  Hardware will dictate a sequence for a certain subset of
    *  _ISR_Handler while requirements for setting
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
   *
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
   *  _Thread_Dispatch_disable_level--;
   *
   *  --_ISR_Nest_level;
   *
   *  if ( _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  if ( _Thread_Dispatch_disable_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  if ( _Thread_Dispatch_necessary ) {
   *    call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   *    prepare to get out of interrupt
   *    return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if outermost interrupt
   *      restore stack
   *  #endif
   *  prepare to get out of interrupt
   *  return from interrupt
   */
/*}    */
	PUBLIC(_ISR_Handler)

SYM(_ISR_Handler):
	ret
