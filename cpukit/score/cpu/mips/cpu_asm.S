/*
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  History:
 *    Baseline: no_cpu
 *    1996:     Ported to MIPS64ORION by Craig Lebakken <craigl@transition.com>
 *          COPYRIGHT (c) 1996 by Transition Networks Inc.
 *          To anyone who acknowledges that the modifications to this file to
 *          port it to the MIPS64ORION are provided "AS IS" without any
 *          express or implied warranty:
 *             permission to use, copy, modify, and distribute this file
 *             for any purpose is hereby granted without fee, provided that
 *             the above copyright notice and this notice appears in all
 *             copies, and that the name of Transition Networks not be used in
 *             advertising or publicity pertaining to distribution of the
 *             software without specific, written prior permission. Transition
 *             Networks makes no representations about the suitability
 *             of this software for any purpose.
 *    2000: Reworked by Alan Cudmore <alanc@linuxstart.com> to become
 *          the more general MIPS port.  Joel Sherrill <joel@OARcorp.com>
 *          continued this rework, rewriting as much as possible in
 *          C and testing on the TX39.
 *  
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <asm.h>
#include "iregdef.h"
#include "idtcpu.h"

#define EXCP_STACK_SIZE (NREGS*R_SZ)
#define ISR_VEC_SIZE 4

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
 *
 *  unsigned32 _CPU_ISR_Get_level( void )
 *
 *  This routine returns the current interrupt level.
 */

#if __mips == 3
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
	mfc0 t0, C0_SR
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
	ori  t0, SR_EXL|SR_IE		/* enable exception level */ 	
	nop
	mtc0 t0,C0_SR         
	nop
_CPU_ISR_Set_exit:
	j ra
	nop
ENDFRAME(_CPU_ISR_Set_level)

#elif __mips == 1

/* MIPS ISA 1 ( R3000 ) */
/* _CPU_ISR_Get/Set_level are called as part of task mode manipulation. */
/*  and are defined in C for the __mips == 1 */

#else
   #error "__mips is set to 1 or 3"
#endif

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
 *   void **fp_context_ptr
 * );
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
 *   void **fp_context_ptr
 * )
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
 *   Context_Control  *run,
 *   Context_Control  *heir
 * )
 */
#if __mips == 3
/* MIPS ISA Level 3 ( R4xxx ) */

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

#elif __mips == 1
/* MIPS ISA Level 1 ( R3000 ) */

FRAME(_CPU_Context_switch,sp,0,ra)

        mfc0 t0,C0_SR
	li t1,~SR_IEC
        sw t0,C0_SR_OFFSET*4(a0)        /* save status register */
        and t0,t1
        mtc0 t0,C0_SR                   /* first disable ie bit (recommended) */

        sw ra,RA_OFFSET*4(a0)           /* save current context */
        sw sp,SP_OFFSET*4(a0)
        sw fp,FP_OFFSET*4(a0)
        sw s0,S0_OFFSET*4(a0)
        sw s1,S1_OFFSET*4(a0)
        sw s2,S2_OFFSET*4(a0)
        sw s3,S3_OFFSET*4(a0)
        sw s4,S4_OFFSET*4(a0)
        sw s5,S5_OFFSET*4(a0)
        sw s6,S6_OFFSET*4(a0)
        sw s7,S7_OFFSET*4(a0)

        mfc0 t0,C0_EPC
        sw t0,C0_EPC_OFFSET*4(a0)

_CPU_Context_switch_restore:
        lw s0,S0_OFFSET*4(a1)           /* restore context */
        lw s1,S1_OFFSET*4(a1)
        lw s2,S2_OFFSET*4(a1)
        lw s3,S3_OFFSET*4(a1)
        lw s4,S4_OFFSET*4(a1)
        lw s5,S5_OFFSET*4(a1)
        lw s6,S6_OFFSET*4(a1)
        lw s7,S7_OFFSET*4(a1)
        lw fp,FP_OFFSET*4(a1)
        lw sp,SP_OFFSET*4(a1)
        lw ra,RA_OFFSET*4(a1)
        lw t0,C0_EPC_OFFSET*4(a1)
        mtc0 t0,C0_EPC
	lw t0, C0_SR_OFFSET*4(a1)
        andi t0,SR_IEC                  /* we know IEC=0, e.g. disabled */
        beq t0,$0,_CPU_Context_1        /* set IEC level from restore context */
	mfc0 t0,C0_SR
        nop
        or  t0,SR_IEC                   /* new_sr = sr | SR_IEC */
        mtc0 t0,C0_SR                   /* set with enabled */
        

_CPU_Context_1:
        j ra
        nop
ENDFRAME(_CPU_Context_switch)

#else

   #error "__mips is not set to 1 or 3"

#endif

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
 *  );
 */

#if __mips == 3

FRAME(_CPU_Context_restore,sp,0,ra)
	dadd a1,a0,zero
	j _CPU_Context_switch_restore
	nop
ENDFRAME(_CPU_Context_restore)

#elif __mips == 1

FRAME(_CPU_Context_restore,sp,0,ra)
	add  a1,a0,zero
	j _CPU_Context_switch_restore
	nop
ENDFRAME(_CPU_Context_restore)

#else

      #error "__mips is not set to 1 or 3"

#endif

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
 *  void _ISR_Handler()
 * 
 *
 *  This discussion ignores a lot of the ugly details in a real
 *  implementation such as saving enough registers/state to be
 *  able to do something real.  Keep in mind that the goal is
 *  to invoke a user's ISR handler which is written in C and
 *  uses a certain set of registers.
 *
 *  Also note that the exact order is to a large extent flexible.
 *  Hardware will dictate a sequence for a certain subset of
 *  _ISR_Handler while requirements for setting
 *
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

#if __mips == 3
/* ----------------------------------------------------------------------------- */
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
        bnez k1,_ISR_Handler_prom_exit /* not an external interrupt,
						pass exception to Monitor */
        mfc0 k1,C0_SR
	and k0,k1
	and k0,CAUSE_IPMASK
	beq k0,zero,_ISR_Handler_quick_exit /* external interrupt not
							enabled, ignore */
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
#if __mips == 1
	la      k0, (R_VEC+((48)*8))
#endif

#if __mips == 3
	la      k0, (R_VEC+((112)*8)) /* R4000 Sim's location is different */
#endif
	j       k0
	nop

       .set    reorder

ENDFRAME(_ISR_Handler)

/* ---------------------------------------------------------------------- */
#elif __mips == 1
/* MIPS ISA Level 1 */

FRAME(_ISR_Handler,sp,0,ra)
.set noreorder

	/* Q: _ISR_Handler, not using IDT/SIM ...save extra regs? */

	addiu sp,sp,-EXCP_STACK_SIZE       /* wastes alot of stack space for context?? */

	sw    ra, R_RA*R_SZ(sp)  /* store ra on the stack */ 
	sw    v0, R_V0*R_SZ(sp)
	sw    v1, R_V1*R_SZ(sp)
	sw    a0, R_A0*R_SZ(sp)
	sw    a1, R_A1*R_SZ(sp)
	sw    a2, R_A2*R_SZ(sp)
	sw    a3, R_A3*R_SZ(sp)
	sw    t0, R_T0*R_SZ(sp)
	sw    t1, R_T1*R_SZ(sp)
	sw    t2, R_T2*R_SZ(sp)
	sw    t3, R_T3*R_SZ(sp)
	sw    t4, R_T4*R_SZ(sp)
	sw    t5, R_T5*R_SZ(sp)
	sw    t6, R_T6*R_SZ(sp)
	sw    t7, R_T7*R_SZ(sp)
	mflo    k0
	sw    t8, R_T8*R_SZ(sp)
	sw    k0, R_MDLO*R_SZ(sp) 
	sw    t9, R_T9*R_SZ(sp)
	mfhi    k0
	sw    gp, R_GP*R_SZ(sp)
	sw    fp, R_FP*R_SZ(sp)
	sw    k0, R_MDHI*R_SZ(sp) 
	.set noat
	sw    AT, R_AT*R_SZ(sp)
	.set at

        /* Q: Why hardcode -40 for stack add??? */
        /* This needs to be figured out.........*/
	addiu sp,sp,-40	
	sw ra,32(sp)	/* store ra on the stack */

/* determine if an interrupt generated this exception */

	mfc0 k0,C0_CAUSE
	and k1,k0,CAUSE_EXCMASK
        beq k1, 0, _ISR_Handler_1
        nop

_ISR_Handler_Exception:
        nop
        b  _ISR_Handler_Exception       /* Jump to the exception code */
        nop

_ISR_Handler_1:

        mfc0 k1,C0_SR
	and k0,k1
	and k0,CAUSE_IPMASK
	beq k0,zero,_ISR_Handler_exit /* external interrupt not enabled, ignore */
	                              /* but if it's not an exception or an interrupt,
				      /* Then where did it come from??? */
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

  /*
   *  _ISR_Nest_level++;
   */
	lw   t0,_ISR_Nest_level
	addi t0,t0,1
	sw   t0,_ISR_Nest_level
  /*
   *  _Thread_Dispatch_disable_level++;
   */
	lw   t1,_Thread_Dispatch_disable_level
	addi t1,t1,1
	sw   t1,_Thread_Dispatch_disable_level

  /*
   *  while ( interrupts_pending(cause_reg) ) {
   *     vector = BITFIELD_TO_INDEX(cause_reg);
   *     (*_ISR_Vector_table[ vector ])( vector );
   *  }
   */
        /* k0 has the SR interrupt bits */
        la t3, _ISR_Vector_table

        /* The bits you look at can be prioritized here just by */
        /*  changing what bit is looked at. I.E. SR_IBITx */
        /* This code might become a loop, servicing all ints before returning.. */
        /*   Right now, it will go thru the whole list once */

_ISR_check_bit_0:
	and k1, k0, SR_IBIT1
        beq k1, zero, _ISR_check_bit_1
        nop
	li  t1, ISR_VEC_SIZE*0 
	add t3, t1
        jal t3  
        nop
_ISR_check_bit_1:
	and k1, k0, SR_IBIT2
        beq k1, zero, _ISR_check_bit_2
        nop
	li  t1, ISR_VEC_SIZE*1
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_2:
	and k1, k0, SR_IBIT3
        beq k1, zero, _ISR_check_bit_3
        nop
	li  t1, ISR_VEC_SIZE*2
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_3:
	and k1, k0, SR_IBIT4
        beq k1, zero, _ISR_check_bit_4
        nop
	li  t1, ISR_VEC_SIZE*3
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_4:
	and k1, k0, SR_IBIT5
        beq k1, zero, _ISR_check_bit_5
        nop
	li  t1, ISR_VEC_SIZE*4
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_5:
	and k1, k0, SR_IBIT6
        beq k1, zero, _ISR_check_bit_6
        nop
	li  t1, ISR_VEC_SIZE*5
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_6:
	and k1, k0, SR_IBIT7
        beq k1, zero, _ISR_check_bit_7
        nop
	li  t1, ISR_VEC_SIZE*6
	add t3, t1
        jal t3 
        nop
_ISR_check_bit_7:
	and k1, k0, SR_IBIT8
        beq k1, zero, _ISR_exit_int_check
        nop
	li  t1, ISR_VEC_SIZE*7
	add t3, t1
        jal t3 
        nop

_ISR_exit_int_check:

  /*
   *  --_ISR_Nest_level;
   */
        lw   t2,_ISR_Nest_level
	addi t2,t2,-1
        sw   t2,_ISR_Nest_level
  /*
   *  --_Thread_Dispatch_disable_level;
   */
	lw   t1,_Thread_Dispatch_disable_level
	addi t1,t1,-1
	sw   t1,_Thread_Dispatch_disable_level
  /*
   *  if ( _Thread_Dispatch_disable_level || _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   */
	or  t0,t2,t1
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
	lw  t0,_Context_Switch_necessary
	lw  t1,_ISR_Signals_to_thread_executing
	or  t0,t0,t1
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
	addiu sp,sp,40    /* Q: Again with the 40...Is this needed? */

/* restore interrupt context from stack */
      
	lw    k0, R_MDLO*R_SZ(sp)           
	mtlo  k0
	lw    k0, R_MDHI*R_SZ(sp)           
	lw    a2, R_A2*R_SZ(sp)
	mthi  k0
	lw    a3, R_A3*R_SZ(sp)
	lw    t0, R_T0*R_SZ(sp)
	lw    t1, R_T1*R_SZ(sp)
	lw    t2, R_T2*R_SZ(sp)
	lw    t3, R_T3*R_SZ(sp)
	lw    t4, R_T4*R_SZ(sp)
	lw    t5, R_T5*R_SZ(sp)
	lw    t6, R_T6*R_SZ(sp)
	lw    t7, R_T7*R_SZ(sp)
	lw    t8, R_T8*R_SZ(sp)
	lw    t9, R_T9*R_SZ(sp)
	lw    gp, R_GP*R_SZ(sp)
	lw    fp, R_FP*R_SZ(sp)
	lw    ra, R_RA*R_SZ(sp)
	lw    a0, R_A0*R_SZ(sp)
	lw    a1, R_A1*R_SZ(sp)
	lw    v1, R_V1*R_SZ(sp)
	lw    v0, R_V0*R_SZ(sp)
	.set noat
	lw    AT, R_AT*R_SZ(sp)
	.set at

	addiu sp,sp,EXCP_STACK_SIZE 
	
        rfe  /* Might not need to do RFE here... */
	j ra
	nop

       .set    reorder
ENDFRAME(_ISR_Handler)

#else

   #error "__mips is not set to 1 or 3 "

#endif

FRAME(mips_break,sp,0,ra)
#if 1
	break 0x0
	j mips_break
#else
	j ra
#endif
	nop
ENDFRAME(mips_break)

