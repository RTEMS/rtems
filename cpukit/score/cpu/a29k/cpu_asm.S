;/*  cpu_asm.c  ===> cpu_asm.S or cpu_asm.s
; *
; *  Author:  Craig Lebakken <craigl@transition.com>
; *
; *  COPYRIGHT (c) 1996 by Transition Networks Inc.
; *
; *  To anyone who acknowledges that this file is provided "AS IS"
; *  without any express or implied warranty:
; *   permission to use, copy, modify, and distribute this file
; *   for any purpose is hereby granted without fee, provided that
; *   the above copyright notice and this notice appears in all
; *   copies, and that the name of Transition Networks not be used in
; *   advertising or publicity pertaining to distribution of the
; *   software without specific, written prior permission.
; *   Transition Networks makes no representations about the suitability
; *   of this software for any purpose.
; *
; *
; *  This file contains the basic algorithms for all assembly code used
; *  in an specific CPU port of RTEMS.  These algorithms must be implemented
; *  in assembly language
; *
; *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
; *
; *  COPYRIGHT (c) 1989-1999.
; *  On-Line Applications Research Corporation (OAR).
; *
; *  The license and distribution terms for this file may be
; *  found in the file LICENSE in this distribution or at
; *  http://www.rtems.com/license/LICENSE.
; *
; *  $Id$
; */

;/*
; *  This is supposed to be an assembly file.  This means that system.h
; *  and cpu.h should not be included in a "real" cpu_asm file.  An
; *  implementation in assembly should include "cpu_asm.h>
; */

;#include <cpu_asm.h>
#include <register.ah>
#include <amd.ah>
#include <pswmacro.ah>
;	.extern _bsp_exit 
;
; push a register onto the struct
        .macro  spush, sp, reg
        store   0, 0, reg, sp           ; push register
        add     sp, sp, 4               ; adjust stack pointer
        .endm
; push a register onto the struct
        .macro  spushsr, sp, reg, sr
	mfsr	reg, sr
        store   0, 0, reg, sp           ; push register
        add     sp, sp, 4               ; adjust stack pointer
        .endm
; pop a register from the struct
        .macro  spop, reg, sp
        load    0, 0, reg, sp
	add	sp,sp,4
        .endm
; pop a special register from the struct
	.macro  spopsr, sreg, reg, sp
	load    0, 0, reg, sp
	mtsr    sreg, reg
	add	sp,sp,4
	.endm
;
;/*
; *  _CPU_Context_save_fp_context
; *
; *  This routine is responsible for saving the FP context
; *  at *fp_context_ptr.  If the point to load the FP context
; *  from is changed then the pointer is modified by this routine.
; *
; *  Sometimes a macro implementation of this is in cpu.h which dereferences
; *  the ** and a similarly named routine in this file is passed something
; *  like a (Context_Control_fp *).  The general rule on making this decision
; *  is to avoid writing assembly language.
; */

;#if 0
;void _CPU_Context_save_fp(
;  void **fp_context_ptr
;)
;{
;}
;#endif
	.global _CPU_Context_save_fp
_CPU_Context_save_fp:
	jmpi	lr0
	nop

;/*
; *  _CPU_Context_restore_fp_context
; *
; *  This routine is responsible for restoring the FP context
; *  at *fp_context_ptr.  If the point to load the FP context
; *  from is changed then the pointer is modified by this routine.
; *
; *  Sometimes a macro implementation of this is in cpu.h which dereferences
; *  the ** and a similarly named routine in this file is passed something
; *  like a (Context_Control_fp *).  The general rule on making this decision
; *  is to avoid writing assembly language.
; */

;#if 0
;void _CPU_Context_restore_fp(
;  void **fp_context_ptr
;)
;{
;}
;#endif
	.global __CPU_Context_restore_fp
__CPU_Context_restore_fp:
	jmpi	lr0
	nop

;/*  _CPU_Context_switch
; *
; *  This routine performs a normal non-FP context switch.
; */
;#if 0
;void _CPU_Context_switch(
;  Context_Control  *run,
;  Context_Control  *heir
;)
;{
;}
;#endif
        .global __CPU_Context_switch
__CPU_Context_switch:
        asneq   106, gr1, gr1                    	; syscall
        jmpi    lr0                                     ;
        nop                                             ;



	.global	_a29k_context_switch_sup
_a29k_context_switch_sup:
#if 0
	add     pcb,lr2,0
	add     kt1,lr3,0	;move heir pointer to safe location
	constn  it0,SIG_SYNC
	spush   pcb,it0
	spush   pcb,gr1
	spush   pcb,rab     	;push rab
	spushsr pcb,it0,pc0 	;push specials
	spushsr pcb,it0,pc1
	add     pcb,pcb,1*4     ;space pc2
	spushsr pcb,it0,CHA	;push CHA
	spushsr pcb,it0,CHD  	;push CHD
	spushsr pcb,it0,CHC 	;push CHC
	add     pcb,pcb,1*4     ;space for alu
	spushsr	pcb,it0,ops     ;push OPS
	mfsr    kt0,cps         ;current status
	const   it1,FZ          ;FZ constant
	andn    it1,kt0,it1     ;clear FZ bit
	mtsr    cps,it1         ;cps without FZ
	add     pcb,pcb,1*4     ;space for tav
	mtsrim  chc,0           ;possible DERR
;
	spush   pcb,lr1 	;push R-stack
	spush   pcb,rfb 	; support
	spush   pcb,msp 	;push M-stack pnt.
;
	add	pcb,pcb,3*4	;space for floating point
;	spush   pcb,FPStat0 	;floating point
;	spush   pcb,FPStat1
;	spush   pcb,FPStat2
;
	add     pcb,pcb,4*4     ;space for IPA..Q
;
	mtsrim  cr,29-1
	storem  0,0,gr96,pcb   	;push gr96-124, optional
	add     pcb,pcb,29*4    ;space for gr96-124
;
	sub     it0,rfb,gr1     ;get bytes in cache
	srl     it0,it0,2       ;adjust to words
	sub     it0,it0,1
	spush	pcb,it0
	mtsr    cr,it0
	storem  0,0,lr0,pcb     ;save lr0-rfb
;
context_restore:
	add	pcb,kt1,0	;pcb=heir
	add	pcb,pcb,4	;space for signal num
	spop	gr1,pcb		;restore freeze registers
	add	gr1,gr1,0	;alu op
	add	pcb,pcb,9*4	;move past freeze registers
	add	pcb,pcb,1*4	;space for tav
	spop	lr1,pcb
	spop	rfb,pcb
	spop	msp,pcb
;	spop	FPStat0,pcb
;	spop	FPStat1,pcb
;	spop	FPStat2,pcb
	add	pcb,pcb,3*4	;space for floating point
	add	pcb,pcb,4*4	;space for IPA..Q
	mtsrim	cr,29-1
	loadm	0,0,gr96,pcb	;pop gr96-gr124
	add	pcb,pcb,29*4	;space for gr96-124

	spop	it1,pcb		;pop locals count
	mtsr	cr,it1
	loadm	0,0,lr0,pcb	;load locals
	
	add	pcb,kt1,0	;pcb=heir
	mtsr	cps,kt0		;cps with FZ
	nop
	add	pcb,pcb,4	;space for signal num
	spop	gr1,pcb		;restore freeze registers
	add	gr1,gr1,0	;alu op
	spop	rab,pcb
	spopsr	pc0,it1,pcb
	spopsr	pc1,it1,pcb
	add	pcb,pcb,4	;space for pc2
	spopsr	CHA,it1,pcb
	spopsr	CHD,it1,pcb
	spopsr	CHC,it1,pcb
	add	pcb,pcb,4	;space for alu
	spopsr	ops,it1,pcb
        nop
        iret
#endif


;/*
; *  _CPU_Context_restore
; *
; *  This routine is generally used only to restart self in an
; *  efficient manner.  It may simply be a label in _CPU_Context_switch.
; *
; *  NOTE: May be unnecessary to reload some registers.
; */
;#if 0
;void _CPU_Context_restore(
;  Context_Control *new_context
;)
;{
;}
;#endif

        .global __CPU_Context_restore
__CPU_Context_restore:
#if 0
        asneq   107, gr1, gr1                   ; syscall
        jmpi    lr0                                     ;
        nop                                             ;

	.global _a29k_context_restore_sup
_a29k_context_restore_sup:
	add	kt1,lr2,0	;kt1 = restore context
        mfsr    kt0,cps         ;current status
	const   it1,FZ          ;FZ constant
	andn    it1,kt0,it1     ;clear FZ bit
	mtsr    cps,it1         ;cps without FZ
	jmp	context_restore
	nop

        .global _a29k_context_save_sup
_a29k_context_save_sup:
	add	pcb,lr2,0
	constn	it0,SIG_SYNC
	spush	pcb,it0
	spush 	pcb,gr1
	spush	pcb,rab		;push rab
	spushsr pcb,it0,pc0	;push specials
	spushsr	pcb,it0,pc1
	add	pcb,pcb,1*4	;space pc2
	spushsr	pcb,it0,CHA	;push CHA
	spushsr	pcb,it0,CHD	;push CHD
	spushsr	pcb,it0,CHC	;push CHC
	add	pcb,pcb,1*4	;space for alu
	spushsr	pcb,it0,ops	;push OPS
	mfsr	it0,cps		;current status
SaveFZState	it1,it2
	add	pcb,pcb,1*4	;space for tav
	mtsrim	chc,0		;possible DERR
;
	spush 	pcb,lr1		;push R-stack
	spush 	pcb,rfb		; support
	spush	pcb,msp		;push M-stack pnt.
;
	spush	pcb,FPStat0	;floating point
	spush	pcb,FPStat1	
	spush	pcb,FPStat2
;
	add	pcb,pcb,4*4	;space for IPA..Q
;
	mtsrim	cr,29-1
	storem	0,0,gr96,pcb	;push gr96-124, optional
	add	pcb,pcb,29*4	;space for gr96-124
;
	sub 	kt0,rfb,gr1	;get bytes in cache
	srl	kt0,kt0,2	;adjust to words
	sub	kt0,kt0,1	
	spush	pcb,kt0		;push number of words
	mtsr	cr,kt0
	storem	0,0,lr0,pcb	;save lr0-rfb
;
	mtsr	cps,it0		;cps with FZ
RestoreFZState	it1,it2

	nop
	nop
	nop
;
	iret
;
#endif

        .global __CPU_Context_save
__CPU_Context_save:
#if 0
        asneq   108, gr1, gr1                   ; syscall
        jmpi    lr0                                     ;
        nop                                             ;
#endif


;/*  void __ISR_Handler()
; *
; *  This routine provides the RTEMS interrupt management.
; *
; */

;#if 0
;void _ISR_Handler()
;{
;   /*
;    *  This discussion ignores a lot of the ugly details in a real
;    *  implementation such as saving enough registers/state to be
;    *  able to do something real.  Keep in mind that the goal is
;    *  to invoke a user's ISR handler which is written in C and
;    *  uses a certain set of registers.
;    *
;    *  Also note that the exact order is to a large extent flexible.
;    *  Hardware will dictate a sequence for a certain subset of
;    *  _ISR_Handler while requirements for setting
;    */

;  /*
;   *  At entry to "common" _ISR_Handler, the vector number must be
;   *  available.  On some CPUs the hardware puts either the vector
;   *  number or the offset into the vector table for this ISR in a
;   *  known place.  If the hardware does not give us this information,
;   *  then the assembly portion of RTEMS for this port will contain
;   *  a set of distinct interrupt entry points which somehow place
;   *  the vector number in a known place (which is safe if another
;   *  interrupt nests this one) and branches to _ISR_Handler.
;   *
;   *  save some or all context on stack
;   *  may need to save some special interrupt information for exit
;   *
;   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
;   *    if ( _ISR_Nest_level == 0 )
;   *      switch to software interrupt stack
;   *  #endif
;   *
;   *  _ISR_Nest_level++;
;   *
;   *  _Thread_Dispatch_disable_level++;
;   *
;   *  (*_ISR_Vector_table[ vector ])( vector );
;   *
;   *  --_ISR_Nest_level;
;   *
;   *  if ( _ISR_Nest_level )
;   *    goto the label "exit interrupt (simple case)"
;   *
;   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
;   *    restore stack
;   *  #endif
;   *  
;   *  if ( !_Context_Switch_necessary )
;   *    goto the label "exit interrupt (simple case)"
;   *  
;   *  if ( !_ISR_Signals_to_thread_executing )
;   *    goto the label "exit interrupt (simple case)"
;   *
;   *  call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
;   *
;   *  prepare to get out of interrupt
;   *  return from interrupt  (maybe to _ISR_Dispatch)
;   *
;   *  LABEL "exit interrupt (simple case):
;   *  prepare to get out of interrupt
;   *  return from interrupt
;   */
;}
;#endif
;        .global __ISR_Handler
;__ISR_Handler:
;	jmpi lr0
;        nop

	.global	_a29k_getops
_a29k_getops:
#if 0
	asneq 113, gr96, gr96
	jmpi lr0
	nop
#endif

	.global	_a29k_getops_sup
_a29k_getops_sup:
#if 0
	mfsr	gr96, ops	; caller wants ops
	iret
	nop
#endif

	.global	_a29k_disable
_a29k_disable:
#if 0
	asneq 110, gr96, gr96
	jmpi lr0
	nop
#endif

	.global	_a29k_disable_sup
_a29k_disable_sup:
#if 0
	mfsr	kt0, ops
	add	gr96, kt0, 0	; return ops to caller
	const	kt1, (DI | TD)
	consth	kt1, (DI | TD)
	or	kt1, kt0, kt1
	mtsr	ops, kt1
	iret
	nop
#endif

        .global _a29k_disable_all
_a29k_disable_all:
#if 0
        asneq 112, gr96, gr96
        jmpi lr0
        nop
#endif

        .global _a29k_disable_all_sup
_a29k_disable_all_sup:
#if 0
        mfsr    kt0, ops
	const	kt1, (DI | TD)
	consth	kt1, (DI | TD)
        or    	kt1, kt0, kt1
        mtsr    ops, kt1
        iret
        nop
#endif

        .global _a29k_enable_all
_a29k_enable_all:
#if 0
        asneq 111, gr96, gr96
        jmpi lr0
        nop
#endif

        .global _a29k_enable_all_sup
_a29k_enable_all_sup:
#if 0
        mfsr    kt0, ops
	const	kt1, (DI | TD)
	consth	kt1, (DI | TD)
        andn    kt1, kt0, kt1
        mtsr    ops, kt1
        iret
        nop
#endif

	.global	_a29k_enable
_a29k_enable:
#if 0
	asneq 109, gr96, gr96
	jmpi lr0
	nop
#endif

	.global	_a29k_enable_sup
_a29k_enable_sup:
#if 0
	mfsr	kt0, ops
	const	kt1, (DI | TD)
	consth	kt1, (DI | TD)
	and	kt3, lr2, kt1
	andn	kt0, kt0, kt1
	or	kt1, kt0, kt3
	mtsr	ops, kt1
	iret
	nop
#endif

	.global _a29k_halt
_a29k_halt:
#if 0
	halt
	jmp _a29k_halt
	nop
#endif

	.global _a29k_super_mode
_a29k_super_mode:
#if 0
	mfsr gr96, ops
	or gr96, gr96, 0x10
	mtsr ops, gr96
	iret
	nop
#endif

	.global _a29k_as70
_a29k_as70:
#if 0
	asneq 70,gr96,gr96
	jmpi lr0
	nop
#endif
