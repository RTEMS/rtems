/*  cpu_asm.S 
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from source copyrighted as follows:
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
/* @(#)cpu_asm.S       08/20/96     1.15 */

#include "cpu_asm.h"

#include "iregdef.h"
#include "idtcpu.h"

#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  name;                           \
        .ent    name;                           \
name:;                                          \
        .frame  frm_reg,offset,ret_reg
#define ENDFRAME(name)                          \
        .end name


#define EXCP_STACK_SIZE (NREGS*R_SZ)

#if __ghs__
#define sd sw
#define ld lw
#define dmtc0 mtc0
#define dsll sll
#define dmfc0 mfc0
#endif

#if 1  /* 32 bit unsigned32 types */
#define sint sw
#define lint lw
#define stackadd addiu
#define intadd addu
#define SZ_INT 4
#define SZ_INT_POW2 2
#else /* 64 bit unsigned32 types */
#define sint dw
#define lint dw
#define stackadd daddiu
#define intadd daddu
#define SZ_INT 8
#define SZ_INT_POW2 3
#endif

#ifdef __GNUC__
#define EXTERN(x,size) .extern x,size
#else
#define EXTERN(x,size)
#endif

/* NOTE: these constants must match the Context_Control structure in cpu.h */
#define S0_OFFSET 0
#define S1_OFFSET 1
#define S2_OFFSET 2
#define S3_OFFSET 3
#define S4_OFFSET 4
#define S5_OFFSET 5
#define S6_OFFSET 6
#define S7_OFFSET 7
#define SP_OFFSET 8
#define FP_OFFSET 9
#define RA_OFFSET 10
#define C0_SR_OFFSET 11
#define C0_EPC_OFFSET 12

/* NOTE: these constants must match the Context_Control_fp structure in cpu.h */
#define FP0_OFFSET  0 
#define FP1_OFFSET  1 
#define FP2_OFFSET  2 
#define FP3_OFFSET  3 
#define FP4_OFFSET  4 
#define FP5_OFFSET  5 
#define FP6_OFFSET  6 
#define FP7_OFFSET  7 
#define FP8_OFFSET  8 
#define FP9_OFFSET  9 
#define FP10_OFFSET 10 
#define FP11_OFFSET 11 
#define FP12_OFFSET 12 
#define FP13_OFFSET 13 
#define FP14_OFFSET 14 
#define FP15_OFFSET 15 
#define FP16_OFFSET 16 
#define FP17_OFFSET 17 
#define FP18_OFFSET 18 
#define FP19_OFFSET 19 
#define FP20_OFFSET 20 
#define FP21_OFFSET 21 
#define FP22_OFFSET 22 
#define FP23_OFFSET 23 
#define FP24_OFFSET 24 
#define FP25_OFFSET 25 
#define FP26_OFFSET 26 
#define FP27_OFFSET 27 
#define FP28_OFFSET 28 
#define FP29_OFFSET 29 
#define FP30_OFFSET 30 
#define FP31_OFFSET 31 


/*PAGE
 *
 *  _CPU_ISR_Get_level
 */

#if 0
unsigned32 _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */
}
#endif
/* return the current exception level for the 4650 */
FRAME(_CPU_ISR_Get_level,sp,0,ra)
	mfc0 v0,C0_SR
	nop
	andi v0,SR_EXL 
	srl v0,1
	j ra
ENDFRAME(_CPU_ISR_Get_level)

FRAME(_CPU_ISR_Set_level,sp,0,ra)
	nop
	mfc0 v0,C0_SR
	nop
	andi v0,SR_EXL 
	beqz v0,_CPU_ISR_Set_1		/* normalize v0 */
	nop
	li v0,1         
_CPU_ISR_Set_1:
	beq v0,a0,_CPU_ISR_Set_exit	/* if (current_level != new_level ) */
	nop
	bnez a0,_CPU_ISR_Set_2
	nop
	nop
	mfc0 t0,C0_SR
	nop
	li t1,~SR_EXL
	and t0,t1
	nop
	mtc0 t0,C0_SR                   /* disable exception level */
	nop
	j ra 
	nop
_CPU_ISR_Set_2:
	nop
	mfc0 t0,C0_SR
	nop
	li t1,~SR_IE
	and t0,t1
	nop
	mtc0 t0,C0_SR                   /* first disable ie bit (recommended) */
	nop
	ori t0,SR_EXL|SR_IE		/* enable exception level  */
	nop
	mtc0 t0,C0_SR         
	nop
_CPU_ISR_Set_exit:
	j ra
	nop
ENDFRAME(_CPU_ISR_Set_level)

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
 */

/* void _CPU_Context_save_fp(
 * void **fp_context_ptr
 * )
 * {
 * }
 */

FRAME(_CPU_Context_save_fp,sp,0,ra)
	.set noat
	ld a1,(a0)
	swc1 $f0,FP0_OFFSET*4(a1)
	swc1 $f1,FP1_OFFSET*4(a1)
	swc1 $f2,FP2_OFFSET*4(a1)
	swc1 $f3,FP3_OFFSET*4(a1)
	swc1 $f4,FP4_OFFSET*4(a1)
	swc1 $f5,FP5_OFFSET*4(a1)
	swc1 $f6,FP6_OFFSET*4(a1)
	swc1 $f7,FP7_OFFSET*4(a1)
	swc1 $f8,FP8_OFFSET*4(a1)
	swc1 $f9,FP9_OFFSET*4(a1)
	swc1 $f10,FP10_OFFSET*4(a1)
	swc1 $f11,FP11_OFFSET*4(a1)
	swc1 $f12,FP12_OFFSET*4(a1)
	swc1 $f13,FP13_OFFSET*4(a1)
	swc1 $f14,FP14_OFFSET*4(a1)
	swc1 $f15,FP15_OFFSET*4(a1)
	swc1 $f16,FP16_OFFSET*4(a1)
	swc1 $f17,FP17_OFFSET*4(a1)
	swc1 $f18,FP18_OFFSET*4(a1)
	swc1 $f19,FP19_OFFSET*4(a1)
	swc1 $f20,FP20_OFFSET*4(a1)
	swc1 $f21,FP21_OFFSET*4(a1)
	swc1 $f22,FP22_OFFSET*4(a1)
	swc1 $f23,FP23_OFFSET*4(a1)
	swc1 $f24,FP24_OFFSET*4(a1)
	swc1 $f25,FP25_OFFSET*4(a1)
	swc1 $f26,FP26_OFFSET*4(a1)
	swc1 $f27,FP27_OFFSET*4(a1)
	swc1 $f28,FP28_OFFSET*4(a1)
	swc1 $f29,FP29_OFFSET*4(a1)
	swc1 $f30,FP30_OFFSET*4(a1)
	swc1 $f31,FP31_OFFSET*4(a1)
	j ra
	nop
	.set at
ENDFRAME(_CPU_Context_save_fp)

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
 */

/* void _CPU_Context_restore_fp(
 * void **fp_context_ptr
 * )
 * {
 * }
 */

FRAME(_CPU_Context_restore_fp,sp,0,ra)
	.set noat
	ld a1,(a0)
	lwc1 $f0,FP0_OFFSET*4(a1)
	lwc1 $f1,FP1_OFFSET*4(a1)
	lwc1 $f2,FP2_OFFSET*4(a1)
	lwc1 $f3,FP3_OFFSET*4(a1)
	lwc1 $f4,FP4_OFFSET*4(a1)
	lwc1 $f5,FP5_OFFSET*4(a1)
	lwc1 $f6,FP6_OFFSET*4(a1)
	lwc1 $f7,FP7_OFFSET*4(a1)
	lwc1 $f8,FP8_OFFSET*4(a1)
	lwc1 $f9,FP9_OFFSET*4(a1)
	lwc1 $f10,FP10_OFFSET*4(a1)
	lwc1 $f11,FP11_OFFSET*4(a1)
	lwc1 $f12,FP12_OFFSET*4(a1)
	lwc1 $f13,FP13_OFFSET*4(a1)
	lwc1 $f14,FP14_OFFSET*4(a1)
	lwc1 $f15,FP15_OFFSET*4(a1)
	lwc1 $f16,FP16_OFFSET*4(a1)
	lwc1 $f17,FP17_OFFSET*4(a1)
	lwc1 $f18,FP18_OFFSET*4(a1)
	lwc1 $f19,FP19_OFFSET*4(a1)
	lwc1 $f20,FP20_OFFSET*4(a1)
	lwc1 $f21,FP21_OFFSET*4(a1)
	lwc1 $f22,FP22_OFFSET*4(a1)
	lwc1 $f23,FP23_OFFSET*4(a1)
	lwc1 $f24,FP24_OFFSET*4(a1)
	lwc1 $f25,FP25_OFFSET*4(a1)
	lwc1 $f26,FP26_OFFSET*4(a1)
	lwc1 $f27,FP27_OFFSET*4(a1)
	lwc1 $f28,FP28_OFFSET*4(a1)
	lwc1 $f29,FP29_OFFSET*4(a1)
	lwc1 $f30,FP30_OFFSET*4(a1)
	lwc1 $f31,FP31_OFFSET*4(a1)
	j ra
	nop
	.set at
ENDFRAME(_CPU_Context_restore_fp)

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 */

/* void _CPU_Context_switch(
 * Context_Control  *run,
 * Context_Control  *heir
 * )
 * {
 * }
 */

FRAME(_CPU_Context_switch,sp,0,ra)

	mfc0 t0,C0_SR
	li t1,~SR_IE
	sd t0,C0_SR_OFFSET*8(a0)	/* save status register */
	and t0,t1
	mtc0 t0,C0_SR                   /* first disable ie bit (recommended) */
	ori t0,SR_EXL|SR_IE		/* enable exception level to disable interrupts */
	mtc0 t0,C0_SR

	sd ra,RA_OFFSET*8(a0)           /* save current context */
	sd sp,SP_OFFSET*8(a0)
	sd fp,FP_OFFSET*8(a0)
	sd s0,S0_OFFSET*8(a0)
	sd s1,S1_OFFSET*8(a0)
	sd s2,S2_OFFSET*8(a0)
	sd s3,S3_OFFSET*8(a0)
	sd s4,S4_OFFSET*8(a0)
	sd s5,S5_OFFSET*8(a0)
	sd s6,S6_OFFSET*8(a0)
	sd s7,S7_OFFSET*8(a0)
	dmfc0 t0,C0_EPC
	sd t0,C0_EPC_OFFSET*8(a0)

_CPU_Context_switch_restore:
	ld s0,S0_OFFSET*8(a1)		/* restore context */
	ld s1,S1_OFFSET*8(a1)
	ld s2,S2_OFFSET*8(a1)
	ld s3,S3_OFFSET*8(a1)
	ld s4,S4_OFFSET*8(a1)
	ld s5,S5_OFFSET*8(a1)
	ld s6,S6_OFFSET*8(a1)
	ld s7,S7_OFFSET*8(a1)
	ld fp,FP_OFFSET*8(a1) 
	ld sp,SP_OFFSET*8(a1)
	ld ra,RA_OFFSET*8(a1)
	ld t0,C0_EPC_OFFSET*8(a1)
	dmtc0 t0,C0_EPC
	ld t0,C0_SR_OFFSET*8(a1)
	andi t0,SR_EXL
	bnez t0,_CPU_Context_1		/* set exception level from restore context */
	li t0,~SR_EXL
	mfc0 t1,C0_SR
	nop
	and t1,t0
	mtc0 t1,C0_SR
_CPU_Context_1:
	j ra
	nop
ENDFRAME(_CPU_Context_switch)

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 */

#if 0
void _CPU_Context_restore(
  Context_Control *new_context
)
{
}
#endif

FRAME(_CPU_Context_restore,sp,0,ra)
	dadd a1,a0,zero
	j _CPU_Context_switch_restore
	nop
ENDFRAME(_CPU_Context_restore)

EXTERN(_ISR_Nest_level, SZ_INT)
EXTERN(_Thread_Dispatch_disable_level,SZ_INT)
EXTERN(_Context_Switch_necessary,SZ_INT)
EXTERN(_ISR_Signals_to_thread_executing,SZ_INT)
.extern _Thread_Dispatch
.extern _ISR_Vector_table

/*  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 */

#if 0
void _ISR_Handler()
{
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
   */
#endif
FRAME(_ISR_Handler,sp,0,ra)
.set noreorder
#if USE_IDTKIT
/* IDT/Kit incorrectly adds 4 to EPC before returning.  This compensates */
	lreg    k0, R_EPC*R_SZ(sp)
	daddiu    k0,k0,-4
	sreg    k0, R_EPC*R_SZ(sp)
	lreg    k0, R_CAUSE*R_SZ(sp)
	li	k1, ~CAUSE_BD
	and     k0, k1 
	sreg    k0, R_CAUSE*R_SZ(sp)
#endif
	
/* save registers not already saved by IDT/sim */
	stackadd sp,sp,-EXCP_STACK_SIZE /* store ra on the stack */

	sreg    ra, R_RA*R_SZ(sp)
	sreg    v0, R_V0*R_SZ(sp)
	sreg    v1, R_V1*R_SZ(sp)
	sreg    a0, R_A0*R_SZ(sp)
	sreg    a1, R_A1*R_SZ(sp)
	sreg    a2, R_A2*R_SZ(sp)
	sreg    a3, R_A3*R_SZ(sp)
	sreg    t0, R_T0*R_SZ(sp)
	sreg    t1, R_T1*R_SZ(sp)
	sreg    t2, R_T2*R_SZ(sp)
	sreg    t3, R_T3*R_SZ(sp)
	sreg    t4, R_T4*R_SZ(sp)
	sreg    t5, R_T5*R_SZ(sp)
	sreg    t6, R_T6*R_SZ(sp)
	sreg    t7, R_T7*R_SZ(sp)
	mflo    k0
	sreg    t8, R_T8*R_SZ(sp)
	sreg    k0, R_MDLO*R_SZ(sp)
	sreg    t9, R_T9*R_SZ(sp)
	mfhi    k0
	sreg    gp, R_GP*R_SZ(sp)
	sreg    fp, R_FP*R_SZ(sp)
	sreg    k0, R_MDHI*R_SZ(sp)
	.set noat
	sreg    AT, R_AT*R_SZ(sp)
	.set at

	stackadd sp,sp,-40		/* store ra on the stack */
	sd ra,32(sp)

/* determine if an interrupt generated this exception */
	mfc0 k0,C0_CAUSE
	and k1,k0,CAUSE_EXCMASK
	bnez k1,_ISR_Handler_prom_exit /* not an external interrupt, pass exception to Monitor */
	mfc0 k1,C0_SR
	and k0,k1
	and k0,CAUSE_IPMASK
	beq k0,zero,_ISR_Handler_quick_exit /* external interrupt not enabled, ignore */
	nop

  /*
   *  save some or all context on stack
   *  may need to save some special interrupt information for exit
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if ( _ISR_Nest_level == 0 )
   *      switch to software interrupt stack
   *  #endif
   */
#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
	lint t0,_ISR_Nest_level
	beq t0, zero,  _ISR_Handler_1
	nop
	/* switch stacks */	
_ISR_Handler_1:
#else
	lint t0,_ISR_Nest_level
#endif
  /*
   *  _ISR_Nest_level++;
   */
	addi t0,t0,1
	sint t0,_ISR_Nest_level
  /*
   *  _Thread_Dispatch_disable_level++;
   */
	lint t1,_Thread_Dispatch_disable_level
	addi t1,t1,1
	sint t1,_Thread_Dispatch_disable_level
#if 0
	nop
	j _ISR_Handler_4
	nop
  /*
   *  while ( interrupts_pending(cause_reg) ) {
   *     vector = BITFIELD_TO_INDEX(cause_reg);
   *     (*_ISR_Vector_table[ vector ])( vector );
   *  }
   */
_ISR_Handler_2:
/* software interrupt priorities can be applied here */
	li t1,-1
/* convert bit field into interrupt index */
_ISR_Handler_3:
	andi t2,t0,1
	addi t1,1
	beql t2,zero,_ISR_Handler_3
	dsrl t0,1
	li t1,7
	dsll t1,3			/* convert index to byte offset (*8) */
	la t3,_ISR_Vector_table
	intadd t1,t3
	lint t1,(t1)
	jalr t1
	nop
	j _ISR_Handler_5
	nop
_ISR_Handler_4:
	mfc0 t0,C0_CAUSE
	andi t0,CAUSE_IPMASK
	bne t0,zero,_ISR_Handler_2
	dsrl t0,t0,8
_ISR_Handler_5:
#else
	nop
	li t1,7
	dsll t1,t1,SZ_INT_POW2
	la t3,_ISR_Vector_table
	intadd t1,t3
	lint t1,(t1)
	jalr t1
	nop
#endif
  /*
   *  --_ISR_Nest_level;
   */
        lint t2,_ISR_Nest_level
	addi t2,t2,-1
        sint t2,_ISR_Nest_level
  /*
   *  --_Thread_Dispatch_disable_level;
   */
	lint t1,_Thread_Dispatch_disable_level
	addi t1,t1,-1
	sint t1,_Thread_Dispatch_disable_level
  /*
   *  if ( _Thread_Dispatch_disable_level || _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   */
	or t0,t2,t1
	bne t0,zero,_ISR_Handler_exit
	nop
  /*
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    restore stack
   *  #endif
   *  
   *  if ( !_Context_Switch_necessary && !_ISR_Signals_to_thread_executing )
   *    goto the label "exit interrupt (simple case)"
   */
	lint t0,_Context_Switch_necessary
	lint t1,_ISR_Signals_to_thread_executing
	or t0,t0,t1
	beq t0,zero,_ISR_Handler_exit
	nop

  /*
   *  call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   */
	jal _Thread_Dispatch
	nop
  /*
   *  prepare to get out of interrupt
   *  return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):
   *  prepare to get out of interrupt
   *  return from interrupt
   */
_ISR_Handler_exit:
	ld ra,32(sp)
	stackadd sp,sp,40

/* restore interrupt context from stack */
	lreg    k0, R_MDLO*R_SZ(sp)
	mtlo    k0
	lreg    k0, R_MDHI*R_SZ(sp)
	lreg    a2, R_A2*R_SZ(sp)
	mthi    k0
	lreg    a3, R_A3*R_SZ(sp)
	lreg    t0, R_T0*R_SZ(sp)
	lreg    t1, R_T1*R_SZ(sp)
	lreg    t2, R_T2*R_SZ(sp)
	lreg    t3, R_T3*R_SZ(sp)
	lreg    t4, R_T4*R_SZ(sp)
	lreg    t5, R_T5*R_SZ(sp)
	lreg    t6, R_T6*R_SZ(sp)
	lreg    t7, R_T7*R_SZ(sp)
	lreg    t8, R_T8*R_SZ(sp)
	lreg    t9, R_T9*R_SZ(sp)
	lreg    gp, R_GP*R_SZ(sp)
	lreg    fp, R_FP*R_SZ(sp)
	lreg    ra, R_RA*R_SZ(sp)
	lreg    a0, R_A0*R_SZ(sp)
	lreg    a1, R_A1*R_SZ(sp)
	lreg    v1, R_V1*R_SZ(sp)
	lreg    v0, R_V0*R_SZ(sp)
	.set noat
	lreg    AT, R_AT*R_SZ(sp)
	.set at

	stackadd sp,sp,EXCP_STACK_SIZE /* store ra on the stack */

#if USE_IDTKIT
/* we handled exception, so return non-zero value */
	li v0,1
#endif

_ISR_Handler_quick_exit:
#ifdef USE_IDTKIT
	j ra
#else
	eret
#endif
	nop

_ISR_Handler_prom_exit:
#ifdef CPU_R3000
	la      k0, (R_VEC+((48)*8))
#endif

#ifdef CPU_R4000
	la      k0, (R_VEC+((112)*8)) /* R4000 Sim's location is different */
#endif
	j       k0
	nop

       .set    reorder

ENDFRAME(_ISR_Handler)


FRAME(mips_enable_interrupts,sp,0,ra)
	mfc0 t0,C0_SR		/* get status reg */
	nop
	or t0,t0,a0		 
	mtc0 t0,C0_SR		/* save updated status reg */
	j ra
	nop
ENDFRAME(mips_enable_interrupts)

FRAME(mips_disable_interrupts,sp,0,ra)
	mfc0 v0,C0_SR		/* get status reg */
	li t1,SR_IMASK		/* t1 = load interrupt mask word */
	not t0,t1		/* t0 = ~t1 */
	and t0,v0		/* clear imask bits */
	mtc0 t0,C0_SR		/* save status reg */
	and v0,t1		/* mask return value (only return imask bits) */
	jr ra
	nop
ENDFRAME(mips_disable_interrupts)

FRAME(mips_enable_global_interrupts,sp,0,ra)
	mfc0 t0,C0_SR		/* get status reg */
	nop
	ori t0,SR_IE
	mtc0 t0,C0_SR		/* save updated status reg */
	j ra
	nop
ENDFRAME(mips_enable_global_interrupts)

FRAME(mips_disable_global_interrupts,sp,0,ra)
	li t1,SR_IE
	mfc0 t0,C0_SR		/* get status reg */
	not t1
	and t0,t1
	mtc0 t0,C0_SR		/* save updated status reg */
	j ra
	nop
ENDFRAME(mips_disable_global_interrupts)

/* return the value of the status register in v0.  Used for debugging */
FRAME(mips_get_sr,sp,0,ra)
	mfc0 v0,C0_SR
	j ra
	nop
ENDFRAME(mips_get_sr)

FRAME(mips_break,sp,0,ra)
#if 1
	break 0x0
	j mips_break
#else
	j ra
#endif
	nop
ENDFRAME(mips_break)

/*PAGE
 *
 *  _CPU_Internal_threads_Idle_thread_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

FRAME(_CPU_Thread_Idle_body,sp,0,ra)
	wait			/* enter low power mode */
	j _CPU_Thread_Idle_body
	nop
ENDFRAME(_CPU_Thread_Idle_body)

#define VEC_CODE_LENGTH 10*4

/**************************************************************************
**
**	init_exc_vecs() - moves the exception code into the addresses
**			  reserved for exception vectors
**
**	UTLB Miss exception vector at address 0x80000000
**
**	General exception vector at address 0x80000080
**
**	RESET exception vector is at address 0xbfc00000
**
***************************************************************************/

#define INITEXCFRM ((2*4)+4)		/* ra + 2 arguments */
FRAME(init_exc_vecs,sp,0,ra)
/* This code yanked from SIM */
#if defined(CPU_R3000)
	.set	noreorder
	la	t1,exc_utlb_code
	la	t2,exc_norm_code
	li	t3,UT_VEC
	li	t4,E_VEC
	li	t5,VEC_CODE_LENGTH
1:
	lw	t6,0(t1)
	lw	t7,0(t2)
	sw	t6,0(t3)
	sw	t7,0(t4)
	addiu	t1,4
	addiu	t3,4
	addiu	t4,4
	subu	t5,4
	bne	t5,zero,1b
	addiu	t2,4
	move	t5,ra		# assumes clear_cache doesnt use t5
	li	a0,UT_VEC
	jal	clear_cache
	li	a1,VEC_CODE_LENGTH
	nop
	li	a0,E_VEC
	jal	clear_cache
	li	a1,VEC_CODE_LENGTH
	move	ra,t5		# restore ra
	j	ra
	nop
	.set	reorder
#endif
#if defined(CPU_R4000)
	.set reorder
	move	t5,ra		# assumes clear_cache doesnt use t5

	/* TLB exception vector */
	la	t1,exc_tlb_code
	li	t2,T_VEC |K1BASE
	li	t3,VEC_CODE_LENGTH
1:
	lw	t6,0(t1)
	addiu	t1,4
	subu	t3,4
	sw	t6,0(t2)
	addiu	t2,4
	bne	t3,zero,1b

	li	a0,T_VEC
	li	a1,VEC_CODE_LENGTH
	jal	clear_cache

	la	t1,exc_xtlb_code
	li	t2,X_VEC |K1BASE
	li	t3,VEC_CODE_LENGTH
1:
	lw	t6,0(t1)
	addiu	t1,4
	subu	t3,4
	sw	t6,0(t2)
	addiu	t2,4
	bne	t3,zero,1b

	/* extended TLB exception vector */
	li	a0,X_VEC
	li	a1,VEC_CODE_LENGTH
	jal	clear_cache

	/* cache error exception vector */
	la	t1,exc_cache_code
	li	t2,C_VEC |K1BASE
	li	t3,VEC_CODE_LENGTH
1:
	lw	t6,0(t1)
	addiu	t1,4
	subu	t3,4
	sw	t6,0(t2)
	addiu	t2,4
	bne	t3,zero,1b

	li	a0,C_VEC
	li	a1,VEC_CODE_LENGTH
	jal	clear_cache

	/* normal exception vector */
	la	t1,exc_norm_code
	li	t2,E_VEC |K1BASE
	li	t3,VEC_CODE_LENGTH
1:
	lw	t6,0(t1)
	addiu	t1,4
	subu	t3,4
	sw	t6,0(t2)
	addiu	t2,4
	bne	t3,zero,1b

	li	a0,E_VEC
	li	a1,VEC_CODE_LENGTH
	jal	clear_cache

	move	ra,t5		# restore ra
	j	ra
#endif
ENDFRAME(init_exc_vecs)


#if defined(CPU_R4000)
FRAME(exc_tlb_code,sp,0,ra)
#ifdef CPU_R3000
        la      k0, (R_VEC+((48)*8))
#endif

#ifdef CPU_R4000
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim's location is different */
#endif
        j       k0
	nop

ENDFRAME(exc_tlb_code)


FRAME(exc_xtlb_code,sp,0,ra)
#ifdef CPU_R3000
        la      k0, (R_VEC+((48)*8))
#endif

#ifdef CPU_R4000
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim's location is different */
#endif
        j       k0
	nop

ENDFRAME(exc_xtlb_code)


FRAME(exc_cache_code,sp,0,ra)
#ifdef CPU_R3000
        la      k0, (R_VEC+((48)*8))
#endif

#ifdef CPU_R4000
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim's location is different */
#endif
        j       k0
	nop

ENDFRAME(exc_cache_code)


FRAME(exc_norm_code,sp,0,ra)
	la	k0, _ISR_Handler /* generic external int hndlr */
	j	k0
	nop
	subu	sp, EXCP_STACK_SIZE		/* set up local stack frame */
ENDFRAME(exc_norm_code)
#endif

/**************************************************************************
**
** enable_int(mask) - enables interrupts - mask is positioned so it only
**                      needs to be or'ed into the status reg. This
**                      also does some other things !!!! caution should
**                      be used if invoking this while in the middle
**                      of a debugging session where the client may have
**                      nested interrupts.
**
****************************************************************************/
FRAME(enable_int,sp,0,ra)
        .set    noreorder
        mfc0    t0,C0_SR
        or      a0,1
        or      t0,a0
        mtc0    t0,C0_SR
        j       ra
        nop
        .set    reorder
ENDFRAME(enable_int)


/***************************************************************************
**
**      disable_int(mask) - disable the interrupt - mask is the complement
**                          of the bits to be cleared - i.e. to clear ext int
**                          5 the mask would be - 0xffff7fff
**
****************************************************************************/
FRAME(disable_int,sp,0,ra)
        .set    noreorder
        mfc0    t0,C0_SR
        nop
        and     t0,a0
        mtc0    t0,C0_SR
        j       ra
        nop
ENDFRAME(disable_int)


