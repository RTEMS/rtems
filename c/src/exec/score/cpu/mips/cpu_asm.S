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
 *          the baseline of the more general MIPS port.  
 *    2001: Joel Sherrill <joel@OARcorp.com> continued this rework,
 *          rewriting as much as possible in C and added the JMR3904 BSP
 *          so testing could be performed on a simulator.
 *    2001: Greg Menke <gregory.menke@gsfc.nasa.gov>, bench tested ISR
 *	    performance, tweaking this code and the isr vectoring routines
 *          to reduce overhead & latencies.  Added optional
 *	    instrumentation as well.
 *    2002: Greg Menke <gregory.menke@gsfc.nasa.gov>, overhauled cpu_asm.S,
 *          cpu.c and cpu.h to manage FP vs int only tasks, interrupt levels
 *          and deferred FP contexts.
 *    2002: Joel Sherrill <joel@OARcorp.com> enhanced the exception processing
 *          by increasing the amount of context saved/restored.
 *  
 *  COPYRIGHT (c) 1989-2002.
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

/* enable debugging shadow writes to misc ram, this is a vestigal
* Mongoose-ism debug tool- but may be handy in the future so we
* left it in...
*/

#define INSTRUMENT_ISR_VECTORING
//#define INSTRUMENT_EXECUTING_THREAD


	
/*  Ifdefs prevent the duplication of code for MIPS ISA Level 3 ( R4xxx )
 *  and MIPS ISA Level 1 (R3xxx).
 */

#if __mips == 3
/* 64 bit register operations */
#define NOP	
#define ADD	dadd
#define STREG	sd
#define LDREG	ld
#define MFCO	dmfc0
#define MTCO	dmtc0
#define ADDU	addu
#define ADDIU	addiu
#define R_SZ	8
#define F_SZ	8
#define SZ_INT	8
#define SZ_INT_POW2 3

/* XXX if we don't always want 64 bit register ops, then another ifdef */

#elif __mips == 1
/* 32 bit register operations*/
#define NOP	nop
#define ADD	add
#define STREG	sw
#define LDREG	lw
#define MFCO	mfc0
#define MTCO	mtc0
#define ADDU	add
#define ADDIU	addi
#define R_SZ	4
#define F_SZ	4
#define SZ_INT	4
#define SZ_INT_POW2 2
#else
#error "mips assembly: what size registers do I deal with?"
#endif


#define ISR_VEC_SIZE	4
#define EXCP_STACK_SIZE (NREGS*R_SZ)

	
#ifdef __GNUC__
#define ASM_EXTERN(x,size) .extern x,size
#else
#define ASM_EXTERN(x,size)
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
/* #define C0_EPC_OFFSET 12 */

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

#if ( CPU_HARDWARE_FP == FALSE )
FRAME(_CPU_Context_save_fp,sp,0,ra)
        .set noat

#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
	/* 
	** Make sure the FPU is on before we save state.  This code is here
	** because the FPU context switch might occur when an integer
	** task is switching out w/ an FP task switching in, but the current 
	** FPU state was left by a sometime previously scheduled FP task.
	**
	** In non-deferred FP context switch, if the exiting task is FP, then
	** the FPU is already on so we don't need to do this.
	*/
	
	MFC0	t0,C0_SR	
	li	k0,SR_CU1	
	or	t0,k0
	MTC0	t0,C0_SR	
#endif
		
	ld   a1,(a0)
	NOP
	.globl _CPU_Context_save_fp_from_exception
_CPU_Context_save_fp_from_exception:
        swc1 $f0,FP0_OFFSET*F_SZ(a1)
        swc1 $f1,FP1_OFFSET*F_SZ(a1)
        swc1 $f2,FP2_OFFSET*F_SZ(a1)
        swc1 $f3,FP3_OFFSET*F_SZ(a1)
        swc1 $f4,FP4_OFFSET*F_SZ(a1)
        swc1 $f5,FP5_OFFSET*F_SZ(a1)
        swc1 $f6,FP6_OFFSET*F_SZ(a1)
        swc1 $f7,FP7_OFFSET*F_SZ(a1)
        swc1 $f8,FP8_OFFSET*F_SZ(a1)
        swc1 $f9,FP9_OFFSET*F_SZ(a1)
        swc1 $f10,FP10_OFFSET*F_SZ(a1)
        swc1 $f11,FP11_OFFSET*F_SZ(a1)
        swc1 $f12,FP12_OFFSET*F_SZ(a1)
        swc1 $f13,FP13_OFFSET*F_SZ(a1)
        swc1 $f14,FP14_OFFSET*F_SZ(a1)
        swc1 $f15,FP15_OFFSET*F_SZ(a1)
        swc1 $f16,FP16_OFFSET*F_SZ(a1)
        swc1 $f17,FP17_OFFSET*F_SZ(a1)
        swc1 $f18,FP18_OFFSET*F_SZ(a1)
        swc1 $f19,FP19_OFFSET*F_SZ(a1)
        swc1 $f20,FP20_OFFSET*F_SZ(a1)
        swc1 $f21,FP21_OFFSET*F_SZ(a1)
        swc1 $f22,FP22_OFFSET*F_SZ(a1)
        swc1 $f23,FP23_OFFSET*F_SZ(a1)
        swc1 $f24,FP24_OFFSET*F_SZ(a1)
        swc1 $f25,FP25_OFFSET*F_SZ(a1)
        swc1 $f26,FP26_OFFSET*F_SZ(a1)
        swc1 $f27,FP27_OFFSET*F_SZ(a1)
        swc1 $f28,FP28_OFFSET*F_SZ(a1)
        swc1 $f29,FP29_OFFSET*F_SZ(a1)
        swc1 $f30,FP30_OFFSET*F_SZ(a1)
        swc1 $f31,FP31_OFFSET*F_SZ(a1)
        j ra
        nop
        .set at
ENDFRAME(_CPU_Context_save_fp)
#endif

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

#if ( CPU_HARDWARE_FP == FALSE )
FRAME(_CPU_Context_restore_fp,sp,0,ra)
        .set noat
	
	/* 
	** Make sure the FPU is on before we retrieve state.  This code 
	** is here because the FPU context switch might occur when an 
	** integer task is switching out with a FP task switching in.
	*/
	
	MFC0	t0,C0_SR	
	li	k0,SR_CU1	
	or	t0,k0
	MTC0	t0,C0_SR	

	ld a1,(a0)
	NOP
	.globl _CPU_Context_restore_fp_from_exception
_CPU_Context_restore_fp_from_exception:
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
#endif

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 */

/* void _CPU_Context_switch(
 *   Context_Control  *run,
 *   Context_Control  *heir
 * )
 */

FRAME(_CPU_Context_switch,sp,0,ra)

        MFC0  t0,C0_SR
        li    t1,~(SR_INTERRUPT_ENABLE_BITS)
	STREG t0,C0_SR_OFFSET*R_SZ(a0)
        and   t0,t1
#if __mips == 3
        ori   t0,(SR_EXL|SR_IE)		/* enable exception level to disable interrupts */
#endif
        MTC0  t0,C0_SR

        STREG ra,RA_OFFSET*R_SZ(a0)	/* save current context */
        STREG sp,SP_OFFSET*R_SZ(a0)
        STREG fp,FP_OFFSET*R_SZ(a0)
        STREG s0,S0_OFFSET*R_SZ(a0)
        STREG s1,S1_OFFSET*R_SZ(a0)
        STREG s2,S2_OFFSET*R_SZ(a0)
        STREG s3,S3_OFFSET*R_SZ(a0)
        STREG s4,S4_OFFSET*R_SZ(a0)
        STREG s5,S5_OFFSET*R_SZ(a0)
        STREG s6,S6_OFFSET*R_SZ(a0)
        STREG s7,S7_OFFSET*R_SZ(a0)

	/*  EPC is readonly...
        MFC0  t0,C0_EPC
	NOP
        STREG t0,C0_EPC_OFFSET*R_SZ(a0)
	*/

_CPU_Context_switch_restore:
        LDREG ra,RA_OFFSET*R_SZ(a1)	    /* restore context */
        LDREG sp,SP_OFFSET*R_SZ(a1)
        LDREG fp,FP_OFFSET*R_SZ(a1)
        LDREG s0,S0_OFFSET*R_SZ(a1)
        LDREG s1,S1_OFFSET*R_SZ(a1)
        LDREG s2,S2_OFFSET*R_SZ(a1)
        LDREG s3,S3_OFFSET*R_SZ(a1)
        LDREG s4,S4_OFFSET*R_SZ(a1)
        LDREG s5,S5_OFFSET*R_SZ(a1)
        LDREG s6,S6_OFFSET*R_SZ(a1)
        LDREG s7,S7_OFFSET*R_SZ(a1)

	/*  EPC is readonly...
        LDREG t0,C0_EPC_OFFSET*R_SZ(a1)
	NOP
        MTC0  t0,C0_EPC
	*/

        LDREG t0, C0_SR_OFFSET*R_SZ(a1)
	
//	NOP
//#if __mips == 3
//        andi  t0,SR_EXL
//        bnez  t0,_CPU_Context_1   /* set exception level from restore context */
//        li    t0,~SR_EXL
//        MFC0  t1,C0_SR
//        NOP
//        and   t1,t0
//        MTC0  t1,C0_SR
//
//#elif __mips == 1
//
//        andi  t0,(SR_INTERRUPT_ENABLE_BITS) /* we know 0 disabled */
//        beq   t0,$0,_CPU_Context_1          /* set level from restore context */
//        MFC0  t0,C0_SR
//        NOP
//        or    t0,(SR_INTERRUPT_ENABLE_BITS) /* new_sr = old sr with enabled  */
//        MTC0  t0,C0_SR                      /* set with enabled */
//	  NOP

	
/*
** Incorporate the new thread's FP coprocessor state and interrupt mask/enable
** into the status register.  We jump thru the requisite hoops to ensure we 
** maintain all other SR bits as global values.
**
** Get the thread's FPU enable, int mask & int enable bits.  Although we keep the 
** software int enables on a per-task basis, the rtems_task_create
** Interrupt Level & int level manipulation functions cannot enable/disable them, 
** so they are automatically enabled for all tasks.  To turn them off, a thread  
** must itself manipulate the SR register.
*/

#if __mips == 3
	li	k0,(SR_CU1 | SR_IMASK | SR_EXL | SR_IE)
#elif __mips == 1
	li	k0,(SR_CU1 | SR_IMASK | SR_IEC)
#endif
	and	t0,k0		
		
	MFC0	t1,C0_SR	/* grab the current SR */
	not	k0		/* invert k0 so we can clear out the SR bits */
	and	t1,k0

	or	t0,t1		/* setup the new task's SR value */

        MTC0	t0,C0_SR	/* and load the new SR */
	NOP
	
/* _CPU_Context_1: */
        j ra
        NOP
ENDFRAME(_CPU_Context_switch)

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

FRAME(_CPU_Context_restore,sp,0,ra)
        ADD a1,a0,zero
        j   _CPU_Context_switch_restore
        NOP
ENDFRAME(_CPU_Context_restore)

ASM_EXTERN(_ISR_Nest_level, SZ_INT)
ASM_EXTERN(_Thread_Dispatch_disable_level,SZ_INT)
ASM_EXTERN(_Context_Switch_necessary,SZ_INT)
ASM_EXTERN(_ISR_Signals_to_thread_executing,SZ_INT)
ASM_EXTERN(_Thread_Executing,SZ_INT)
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

FRAME(_ISR_Handler,sp,0,ra)
        .set noreorder

        /* Q: _ISR_Handler, not using IDT/SIM ...save extra regs? */

        /* wastes a lot of stack space for context?? */
        ADDIU    sp,sp,-EXCP_STACK_SIZE

        STREG ra, R_RA*R_SZ(sp)  /* store ra on the stack */ 
        STREG v0, R_V0*R_SZ(sp)
        STREG v1, R_V1*R_SZ(sp)
        STREG a0, R_A0*R_SZ(sp)
        STREG a1, R_A1*R_SZ(sp)
        STREG a2, R_A2*R_SZ(sp)
        STREG a3, R_A3*R_SZ(sp)
        STREG t0, R_T0*R_SZ(sp)
        STREG t1, R_T1*R_SZ(sp)
        STREG t2, R_T2*R_SZ(sp)
        STREG t3, R_T3*R_SZ(sp)
        STREG t4, R_T4*R_SZ(sp)
        STREG t5, R_T5*R_SZ(sp)
        STREG t6, R_T6*R_SZ(sp)
        STREG t7, R_T7*R_SZ(sp)
        mflo  t0
        STREG t8, R_T8*R_SZ(sp)
        STREG t0, R_MDLO*R_SZ(sp) 
        STREG t9, R_T9*R_SZ(sp)
        mfhi  t0
        STREG gp, R_GP*R_SZ(sp)
        STREG t0, R_MDHI*R_SZ(sp) 
        STREG fp, R_FP*R_SZ(sp)
        .set noat
        STREG AT, R_AT*R_SZ(sp)
        .set at

        MFC0     t0,C0_SR
	MFC0     t1,C0_EPC
        STREG    t0,R_SR*R_SZ(sp)
        STREG    t1,R_EPC*R_SZ(sp)
	

#ifdef INSTRUMENT_EXECUTING_THREAD
	lw t2, _Thread_Executing
	nop
	sw t2, 0x8001FFF0
#endif
	
	/* determine if an interrupt generated this exception */

        MFC0     k0,C0_CAUSE
	NOP

	and      k1,k0,CAUSE_EXCMASK
        beq      k1, 0, _ISR_Handler_1

_ISR_Handler_Exception:

	/*  If we return from the exception, it is assumed nothing
         *  bad is going on and we can continue to run normally.
         *  But we want to save the entire CPU context so exception
         *  handlers can look at it and change it.
         *
         *  NOTE: This is the path the debugger stub will take.
         */

        STREG    sp,SP_OFFSET*R_SZ(sp)     /* save sp */

        STREG    s0,S0_OFFSET*R_SZ(sp)     /* save s0 - s7 */
        STREG    s1,S1_OFFSET*R_SZ(sp)
        STREG    s2,S2_OFFSET*R_SZ(sp)
        STREG    s3,S3_OFFSET*R_SZ(sp)
        STREG    s4,S4_OFFSET*R_SZ(sp)
        STREG    s5,S5_OFFSET*R_SZ(sp)
        STREG    s6,S6_OFFSET*R_SZ(sp)
        STREG    s7,S7_OFFSET*R_SZ(sp)

        MFC0     k0,C0_CAUSE               /* save cause */
	NOP
        STREG    k0,R_CAUSE*R_SZ(sp)

        /* CP0 special registers */

        MFC0     t0,C0_BADVADDR
        nop
        STREG    t0,R_BADVADDR*R_SZ(sp)

#if ( CPU_HARDWARE_FP == TRUE )
        MFC0     t0,C0_SR                 /* FPU is enabled, save state */
        srl      t0,t0,16
        andi     t0,t0,(SR_CU1 >> 16)
        beqz     t0, 1f
        nop
        
        la       a1,R_F0*R_SZ(sp)
        jal      _CPU_Context_save_fp_from_exception 
        nop
        MFC1     t0,C1_REVISION
        MFC1     t1,C1_STATUS
        STREG    t0,R_FEIR*R_SZ(sp)
        STREG    t1,R_FCSR*R_SZ(sp)
        
1:      
#endif
	move	 a0,sp
        jal	 mips_vector_exceptions
	nop

#if ( CPU_HARDWARE_FP == TRUE )
        MFC0     t0,C0_SR               /* FPU is enabled, restore state */
        srl      t0,t0,16
        andi     t0,t0,(SR_CU1 >> 16)
        beqz     t0, 2f
        nop
        
        la       a1,R_F0*R_SZ(sp)
        jal      _CPU_Context_restore_fp_from_exception
        nop
        LDREG    t0,R_FEIR*R_SZ(sp)
        LDREG    t1,R_FCSR*R_SZ(sp)
        MTC1     t0,C1_REVISION
        MTC1     t1,C1_STATUS
2:
#endif
        LDREG    s0,S0_OFFSET*R_SZ(sp)    /* restore s0 - s7 */
        LDREG    s1,S1_OFFSET*R_SZ(sp)
        LDREG    s2,S2_OFFSET*R_SZ(sp)
        LDREG    s3,S3_OFFSET*R_SZ(sp)
        LDREG    s4,S4_OFFSET*R_SZ(sp)
        LDREG    s5,S5_OFFSET*R_SZ(sp)
        LDREG    s6,S6_OFFSET*R_SZ(sp)
        LDREG    s7,S7_OFFSET*R_SZ(sp)

        /* do NOT restore the sp as this could mess up the world */
        /* do NOT restore the cause as this could mess up the world */

	j	 _ISR_Handler_exit
	nop

_ISR_Handler_1:

        MFC0     k1,C0_SR
        and      k0,CAUSE_IPMASK
        and      k0,k1

        /* external interrupt not enabled, ignore */
        /* but if it's not an exception or an interrupt, */
        /* Then where did it come from??? */
	
	beq      k0,zero,_ISR_Handler_exit

	
	
	
		
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
        LDREG  t0,_ISR_Nest_level
	NOP
        ADD    t0,t0,1
        STREG  t0,_ISR_Nest_level
  /*
   *  _Thread_Dispatch_disable_level++;
   */
        LDREG  t1,_Thread_Dispatch_disable_level
	NOP
        ADD    t1,t1,1
        STREG  t1,_Thread_Dispatch_disable_level

  /*
   *  Call the CPU model or BSP specific routine to decode the
   *  interrupt source and actually vector to device ISR handlers.
   */
	
#ifdef INSTRUMENT_ISR_VECTORING
	nop
	li	t1, 1
	sw	t1, 0x8001e000
#endif

	move	 a0,sp
        jal      mips_vector_isr_handlers
        nop
	
#ifdef INSTRUMENT_ISR_VECTORING
	li	t1, 0
	sw	t1, 0x8001e000
	nop
#endif
		
  /*
   *  --_ISR_Nest_level;
   */
        LDREG  t2,_ISR_Nest_level
	NOP
        ADD    t2,t2,-1
        STREG  t2,_ISR_Nest_level
  /*
   *  --_Thread_Dispatch_disable_level;
   */
        LDREG  t1,_Thread_Dispatch_disable_level
	NOP
        ADD    t1,t1,-1
        STREG  t1,_Thread_Dispatch_disable_level
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
        LDREG t0,_Context_Switch_necessary
        LDREG t1,_ISR_Signals_to_thread_executing
	NOP
        or    t0,t0,t1
        beq   t0,zero,_ISR_Handler_exit
        nop

	
	
#ifdef INSTRUMENT_EXECUTING_THREAD
	lw	t0,_Thread_Executing
	nop
	sw	t0,0x8001FFF4
#endif

/*
** Turn on interrupts before entering Thread_Dispatch which
** will run for a while, thus allowing new interrupts to
** be serviced.  Observe the Thread_Dispatch_disable_level interlock
** that prevents recursive entry into Thread_Dispatch.
*/

        MFC0    t0, C0_SR
	NOP
	or	t0, SR_INTERRUPT_ENABLE_BITS
        MTC0    t0, C0_SR
	NOP
		
        jal     _Thread_Dispatch
        NOP

#ifdef INSTRUMENT_EXECUTING_THREAD
	lw	t0,_Thread_Executing
	nop
	sw	t0,0x8001FFF8
#endif

	
  /*
   *  prepare to get out of interrupt
   *  return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):"
   *  prepare to get out of interrupt
   *  return from interrupt
   */

_ISR_Handler_exit:
/*
** Skip the SR restore because its a global register. _CPU_Context_switch_restore
** adjusts it according to each task's configuration.  If we didn't dispatch, the
** SR value isn't changing, so all we need to do is return.
**
*/

	/* restore context from stack */
	
#ifdef INSTRUMENT_EXECUTING_THREAD
	lw	t0,_Thread_Executing
	nop
	sw	t0, 0x8001FFFC
#endif

        LDREG k0, R_MDLO*R_SZ(sp)
        LDREG t0, R_T0*R_SZ(sp)
        mtlo  k0
        LDREG k0, R_MDHI*R_SZ(sp)           
        LDREG t1, R_T1*R_SZ(sp)
        mthi  k0
        LDREG t2, R_T2*R_SZ(sp)
        LDREG t3, R_T3*R_SZ(sp)
        LDREG t4, R_T4*R_SZ(sp)
        LDREG t5, R_T5*R_SZ(sp)
        LDREG t6, R_T6*R_SZ(sp)
        LDREG t7, R_T7*R_SZ(sp)
        LDREG t8, R_T8*R_SZ(sp)
        LDREG t9, R_T9*R_SZ(sp)
        LDREG gp, R_GP*R_SZ(sp)
        LDREG fp, R_FP*R_SZ(sp)
        LDREG ra, R_RA*R_SZ(sp)
        LDREG a0, R_A0*R_SZ(sp)
        LDREG a1, R_A1*R_SZ(sp)
        LDREG a2, R_A2*R_SZ(sp)
        LDREG a3, R_A3*R_SZ(sp)
        LDREG v1, R_V1*R_SZ(sp)
        LDREG v0, R_V0*R_SZ(sp)
	
        LDREG     k0, R_EPC*R_SZ(sp)
	
	.set noat
        LDREG     AT, R_AT*R_SZ(sp)
        .set at

        ADDIU     sp,sp,EXCP_STACK_SIZE
	j         k0
	rfe
        nop

       .set    reorder
ENDFRAME(_ISR_Handler)

	
FRAME(mips_break,sp,0,ra)
#if 1
        break 0x0 
        j mips_break
#else
        j ra
#endif
        nop
ENDFRAME(mips_break)

