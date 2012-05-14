/*
 * Hitachi H8 Score CPU functions
 *   Copyright Comnet Technologies Ltd 1999
 *
 *  Based on example code and other ports with this copyright:
 *
 *  COPYRIGHT (c) 1989-1999.
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

;.equ	RUNCONTEXT_ARG,	 er0
;.equ	HEIRCONTEXT_ARG, er1
	
/*
 *  Make sure we tell the assembler what type of CPU model we are
 *  being compiled for.
 */

#if defined(__H8300H__)
        .h8300h
#endif
#if defined(__H8300S__)
        .h8300s
#endif
#if defined(__H8300SX__)
        .h8300sx
#endif
        .text

	.text
/*
	GCC Compiled with optimisations and Wimplicit decs to ensure
    that stack from doesn't change
	
	Supposedly R2 and R3 do not need to be saved but who knows
	
	Arg1 = er0 	(not on stack)
	Arg2 = er1	(not on stack)
*/

	.align 2

	.global	SYM(_CPU_Context_switch)

SYM(_CPU_Context_switch):
	/* Save Context */
#if defined(__H8300H__) || defined(__H8300S__) || defined(__H8300SX__)
	stc.w	ccr,@(0:16,er0)
	mov.l	er7,@(2:16,er0)
	mov.l	er6,@(6:16,er0)
	mov.l	er5,@(10:16,er0)
	mov.l	er4,@(14:16,er0)
	mov.l	er3,@(18:16,er0)
	mov.l	er2,@(22:16,er0)

	/* Install New context */

restore:
	mov.l	@(22:16,er1),er2
	mov.l	@(18:16,er1),er3
	mov.l	@(14:16,er1),er4
	mov.l	@(10:16,er1),er5
	mov.l	@(6:16,er1),er6
	mov.l	@(2:16,er1),er7
	ldc.w	@(0:16,er1),ccr
#endif

	rts

	.align 2

	.global	SYM(_CPU_Context_restore)

SYM(_CPU_Context_restore):
	
#if defined(__H8300H__) || defined(__H8300S__) || defined(__H8300SX__)
	mov.l	er0,er1
	jmp		@restore:24
#endif



/*
	VHandler for Vectored Interrupts
	
	All IRQ's are vectored to routine _ISR_#vector_number
	This routine stacks er0 and loads er0 with vector number
	before transferring to here
	
*/
	.align 2
	.global	SYM(_ISR_Handler)
	.extern	SYM(_Vector_table)

	
SYM(_ISR_Handler):
#if defined(__H8300H__) || defined(__H8300S__) || defined(__H8300SX__)
	mov.l	er1,@-er7
	mov.l	er2,@-er7
	mov.l	er3,@-er7
	mov.l	er4,@-er7
	mov.l	er5,@-er7
	mov.l	er6,@-er7
	
/*  Set IRQ Stack */
	orc		#0xc0,ccr
	mov.l	er7,er6		; save stack pointer
	mov.l	@ISR_NEST_LEVEL,er1
	bne		nested
	mov.l	@INTERRUPT_STACK_HIGH,er7
	
nested:
	mov.l	er6,@-er7	; save sp so pop regardless of nest level
	
;; Inc  system counters
	mov.l	@ISR_NEST_LEVEL,er1
	inc.l	#1,er1
	mov.l	er1,@ISR_NEST_LEVEL
	mov.l	@SYM(_Thread_Dispatch_disable_level),er1
	inc.l	#1,er1
	mov.l	er1,@SYM(_Thread_Dispatch_disable_level)
	
/* Vector to ISR */

	mov.l	@SYM(_ISR_Vector_table),er1
	mov		er0,er2	; copy vector
	shll.l	er2
	shll.l	er2		; vector = vector * 4 (sizeof(int))
	add.l	er2,er1
    mov.l	@er1,er1
	jsr		@er1	; er0 = arg1 =vector
	
	orc		#0xc0,ccr
	mov.l	@ISR_NEST_LEVEL,er1
	dec.l	#1,er1
	mov.l	er1,@ISR_NEST_LEVEL
	mov.l	@SYM(_Thread_Dispatch_disable_level),er1
	dec.l	#1,er1
	mov.l	er1,@SYM(_Thread_Dispatch_disable_level)
	bne	exit
	
	mov.b	@DISPATCH_NEEDED,er1
	beq	exit		; If no then exit

	/* Context switch here through ISR_Dispatch */
bframe:
	orc	#0xc0,ccr
/*	Pop Stack	*/
	mov	@er7+,er6
	mov	er6,er7

	/* Set up IRQ stack frame and dispatch to _ISR_Dispatch */
	
	mov.l	#0xc0000000,er2		/* Disable IRQ */
	or.l	#SYM(_ISR_Dispatch),er2
	mov.l	er2,@-er7
	rte

/*	Inner IRQ Return, pop flags and return */
exit:
/*	Pop Stack	*/
	orc		#0x80,ccr
	mov		@er7+,er6
	mov		er6,er7
	mov		@er7+,er6
	mov		@er7+,er5
	mov		@er7+,er4
	mov		@er7+,er3
	mov		@er7+,er2
	mov		@er7+,er1
	mov		@er7+,er0
#endif
	rte
	
/*
	Called from ISR_Handler as a way of ending IRQ
	but allowing dispatch to another task.
	Must use RTE as CCR is still on stack but IRQ has been serviced.	
	CCR and PC occupy same word so rte can be used.
	now using task stack
*/

	.align 2
	.global SYM(_ISR_Dispatch)

SYM(_ISR_Dispatch):
	
#if defined(__H8300H__) || defined(__H8300S__) || defined(__H8300SX__)
	jsr		@SYM(_Thread_Dispatch)
	mov		@er7+,er6
	mov		@er7+,er5
	mov		@er7+,er4
	mov		@er7+,er3
	mov		@er7+,er2
	mov		@er7+,er1
	mov		@er7+,er0
#endif
	rte
	

	.align 2
	.global SYM(_CPU_Context_save_fp)

SYM(_CPU_Context_save_fp):
	rts


	.align 2
	.global SYM(_CPU_Context_restore_fp)

SYM(_CPU_Context_restore_fp):
	rts
