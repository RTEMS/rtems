/*  exception.S
 *
 *  This file contains a customized MIPS exception handler.
 *  It hooks into the exception handler present in the resident
 *  PMON debug monitor.
 *
 *  Author: Bruce Robinson
 *
 *  This code was derived from cpu_asm.S with the following copyright:
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
 *  Derived from c/src/exec/score/cpu/no_cpu/cpu_asm.s:
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#if BSP_HAS_USC320
  #include <usc.h>
#endif

#if __mips == 3
/* 64 bit register operations */
#define NOP	nop
#define ADD	dadd
#define STREG	sd
#define LDREG	ld
#define ADDU	addu
#define ADDIU	addiu
#define STREGC1	sdc1
#define LDREGC1	ldc1
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
#define ADDU	add
#define ADDIU	addi
#define STREGC1	swc1
#define LDREGC1	lwc1
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
#define EXTERN(x,size) .extern x,size
#else
#define EXTERN(x,size)
#endif


EXTERN(_Thread_Dispatch_disable_level,4)
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
FRAME(bsp_ISR_Handler,sp,0,ra)
	.set noreorder

#if 0
/* Activate TX49xx PIO19 signal for diagnostics */
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	lw	k0,(k0)
	lui	k1,0x8
	or	k1,k1,k0
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	sw	k1,(k0)
#endif
	mfc0 k0,C0_CAUSE	/* Determine if an interrupt generated this exception */
	nop
	and k1,k0,CAUSE_EXCMASK
	beq k1,zero,_chk_int	/* If so, branch to service here */
	nop
	la  k0,_int_esr_link	/* Otherwise, jump to next exception handler in PMON exception chain */
	lw  k0,(k0)
	lw  k0,4(k0)
	j   k0
	nop
_chk_int:
	mfc0 k1,C0_SR
	nop
	and k0,k1
#if HAS_RM52xx
	and k0,CAUSE_IPMASK
#elif HAS_TX49xx
	and k0,(SR_IBIT1 | SR_IBIT2 | SR_IBIT3)
#endif
	/* external interrupt not enabled, ignore */
	beq k0,zero,_ISR_Handler_quick_exit
	nop

/* For debugging interrupts, clear EXL to allow breakpoints */
#if 0
        MFC0    k0, C0_SR
#if __mips == 3
	li	k1,SR_EXL	/* Clear EXL and Set IE to enable interrupts */
	not	k1
	and	k0,k1
	li	k1,SR_IE
#elif __mips == 1
	li	k1,SR_IEC
#endif
	or	k0, k1
        mtc0    k0, C0_SR
	NOP
#endif


  /*
   *  save some or all context on stack
   *  may need to save some special interrupt information for exit
   */

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

        mfc0     t0,C0_SR
	dmfc0    t1,C0_EPC
        STREG    t0,R_SR*R_SZ(sp)
        STREG    t1,R_EPC*R_SZ(sp)

  /*
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if ( _ISR_Nest_level == 0 )
   *      switch to software interrupt stack
   *  #endif
   */

  /*
   *  _ISR_Nest_level++;
   */
        lw	t0,ISR_NEST_LEVEL
	NOP
        add	t0,t0,1
        sw	t0,ISR_NEST_LEVEL
  /*
   *  _Thread_Dispatch_disable_level++;
   */
        lw	t1,_Thread_Dispatch_disable_level
	NOP
        add	t1,t1,1
        sw	t1,_Thread_Dispatch_disable_level

	/* DEBUG - Add the following code to disable interrupts and clear
	 *	   EXL in status register, this will allow memory
	 *         exceptions to occur while servicing the current interrupt
	 */
#if 0
	/* Disable interrupts from internal interrupt controller */
	li t0,~CAUSE_IP2_MASK
	mfc0 t1,C0_SR
	nop
	and t1,t0
	mtc0 t1,C0_SR
	nop
	/* Clear EXL in status register to allow memory exceptions to occur */
	li t0,~SR_EXL
	mfc0 t1,C0_SR
	nop
	and t1,t0
	mtc0 t1,C0_SR
	nop
#endif

  /*
   *  Call the CPU model or BSP specific routine to decode the
   *  interrupt source and actually vector to device ISR handlers.
   */
	move	 a0,sp
        jal      mips_vector_isr_handlers
        NOP

	/* Add the following code to disable interrupts (see DEBUG above) */
#if 0
	li t0,SR_EXL		/* Set EXL to hold off interrupts */
	mfc0 t1,C0_SR
	nop
	or t1,t0
	mtc0 t1,C0_SR
	nop
	/* Enable interrupts from internal interrupt controller */
	li t0,CAUSE_IP2_MASK
	mfc0 t1,C0_SR
	nop
	or t1,t0
	mtc0 t1,C0_SR
	nop
#endif

_ISR_Handler_cleanup:

  /*
   *  --_ISR_Nest_level;
   */
        lw	t2,ISR_NEST_LEVEL
	NOP
        add	t2,t2,-1
        sw	t2,ISR_NEST_LEVEL
  /*
   *  --_Thread_Dispatch_disable_level;
   */
        lw	t1,_Thread_Dispatch_disable_level
	NOP
        add	t1,t1,-1
        sw	t1,_Thread_Dispatch_disable_level
  /*
   *  if ( _Thread_Dispatch_disable_level || _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   */
        or  t0,t2,t1
        bne t0,zero,_ISR_Handler_exit
        NOP


  /*
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    restore stack
   *  #endif
   *
   *  if ( !_Thread_Dispatch_necessary )
   *    goto the label "exit interrupt (simple case)"
   */
        lb	t0,DISPATCH_NEEDED
	NOP
        or 	t0,t0,t0
        beq	t0,zero,_ISR_Handler_exit
        NOP

/*
** Turn on interrupts before entering Thread_Dispatch which
** will run for a while, thus allowing new interrupts to
** be serviced.  Observe the Thread_Dispatch_disable_level interlock
** that prevents recursive entry into Thread_Dispatch.
*/

        mfc0    t0, C0_SR
#if __mips == 3
	li	t1,SR_EXL	/* Clear EXL and Set IE to enable interrupts */
	not	t1
	and	t0,t1
	li	t1,SR_IE
#elif __mips == 1
	li	t1,SR_IEC
#endif
	or	t0, t1
        mtc0    t0, C0_SR
	NOP

	/* save off our stack frame so the context switcher can get to it */
	la	t0,__exceptionStackFrame
	STREG	sp,(t0)

        jal     _Thread_Dispatch
        NOP

	/* and make sure its clear in case we didn't dispatch.  if we did, its
	** already cleared */
	la	t0,__exceptionStackFrame
	STREG	zero,(t0)
	NOP

/*
** turn interrupts back off while we restore context so
** a badly timed interrupt won't accidentally mess things up
*/
        mfc0    t0, C0_SR
#if __mips == 3
	li	t1,SR_IE		/* Clear IE first (recommended) */
	not	t1
	and	t0,t1
        mtc0    t0, C0_SR
	li	t1,SR_EXL | SR_IE	/* Set EXL and IE, this puts status register bits back to interrupted state */
	or	t0,t1
#elif __mips == 1
	/* ints off, current & prev kernel mode on (kernel mode enabled is bit clear..argh!) */
	li	t1,SR_IEC | SR_KUP | SR_KUC
	not	t1
	and	t0, t1
#endif
	mtc0    t0, C0_SR
	NOP

  /*
   *  prepare to get out of interrupt
   *  return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):"
   *  prepare to get out of interrupt
   *  return from interrupt
   */

_ISR_Handler_exit:

/* restore interrupt context from stack */
        LDREG t8, R_MDLO*R_SZ(sp)
        LDREG t0, R_T0*R_SZ(sp)
        mtlo  t8
        LDREG t8, R_MDHI*R_SZ(sp)
        LDREG t1, R_T1*R_SZ(sp)
        mthi  t8
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

        LDREG k1, R_EPC*R_SZ(sp)
	mtc0  k1,C0_EPC

	.set noat
        LDREG     AT, R_AT*R_SZ(sp)
        .set at

        ADDIU     sp,sp,EXCP_STACK_SIZE

_ISR_Handler_quick_exit:
	eret
	nop


#if BSP_HAS_USC320
	/* Interrupts from USC320 are serviced here */
	.global	USC_isr
	.extern	Clock_isr
USC_isr:
	/* check if it's a USC320 heartbeat interrupt */
        la      k0,INT_STAT	/* read INT_STAT register */
        lw      k0,(k0)
        nop			/* reading from external device	*/
        sll     k0,(31-21)	/* test bit 21 (HBI) */

        bgez	k0,USC_isr2	/* branch if not a heartbeat interrupt */
	NOP

	/* clear the heartbeat interrupt */
	la      k0,INT_STAT
	li      t0,HBI_MASK
	sw      t0,(k0)
	/* wait for interrupt to clear */
USC_isr1:
	la      k0,INT_STAT	/* read INT_STAT register */
	lw      k0,(k0)
	nop			/* reading from external device */
        sll     k0,(31-21)	/* test bit 21 (HBI) */
        bltz    k0,USC_isr1   	/* branch if bit set */
        nop
	j	Clock_isr	/* Jump to clock isr */
	nop
USC_isr2:
	j	ra		/* no serviceable interrupt, return without doing anything */
	nop
#endif

#if 0
	.global	int7_isr
	.extern	Interrupt_7_isr
int7_isr:
	/* Verify interrupt is from Timer */
        la      k0,IRCS		/* read Interrupt Current Status register */
        lw      k0,(k0)
        nop			/* reading from external device	*/
	li	k1,IRCS_CAUSE_MASK
        and     k0,k0,k1	/* isolate interrupt cause  */

	li	k1,INT7INT	/* test for interrupt 7 */
        subu	k1,k0,k1
        beq	k1,zero,int7_isr1
        nop
        j	ra		/* interrupt 7 no longer valid, return without doing anything */
        nop
int7_isr1:
	j	Interrupt_7_isr	/* Jump to Interrupt 7 isr */
	nop
#endif

       .set    reorder

ENDFRAME(bsp_ISR_Handler)


FRAME(_BRK_Handler,sp,0,ra)
	.set noreorder

#if BSP_HAS_USC320
	la	k0,INT_CFG3	/* Disable heartbeat interrupt in USC320, it interferes with PMON exception handler */
	lw	k1,(k0)
	li	k0,~HBI_MASK
	and	k1,k1,k0
	la	k0,INT_CFG3
	sw	k1,(k0)
#endif

	la  k0,_brk_esr_link	/* Jump to next exception handler in PMON exception chain */
	lw  k0,(k0)
	lw  k0,4(k0)
	j   k0
	nop

	.set reorder
ENDFRAME(_BRK_Handler)


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

FRAME(init_exc_vecs,sp,0,ra)
	.set noreorder

	.extern mon_onintr

/* Install interrupt handler in PMON exception handling chain */

	addiu	sp,sp,-8
	sw	ra,(sp)			/* Save ra contents on stack */
	move	a0,zero
	la	a1,_int_esr_link
	jal	mon_onintr		/* Make PMON system call to install interrupt exception handler */
	nop
	li	a0,9
	la	a1,_brk_esr_link
	jal	mon_onintr		/* Make PMON system call to install break exception handler */
	nop
	lw	ra,(sp)
	addiu	sp,sp,8			/* Restore ra contents from stack */
	j	ra
	nop

	.set reorder
ENDFRAME(init_exc_vecs)


#if 0		/* Unused code below */

/*************************************************************
*  enable_int7(ints)
*	Enable interrupt 7
*/
FRAME(enable_int7,sp,0,ra)
	.set noreorder

	la	t0,IRDM1	# Set interrupt controller detection mode (bits 2-3 = 0 for int 7 active low)
	li	t1,0x0
	sw	t1,(t0)

	la	t0,IRLVL4	# Set interrupt controller level (bit 8-10 = 2 for int 7 at level 2)
	li	t1,0x200
	sw	t1,(t0)

	la	t0,IRMSK	# Set interrupt controller mask
	li	t1,0x0
	sw	t1,(t0)

	la	t0,IRDEN	# Enable interrupts from controller
	li	t1,0x1
	sw	t1,(t0)

	j	ra
	nop
	.set reorder
ENDFRAME(enable_int7)

/*************************************************************
*  disable_int7(ints)
*	Disable interrupt 7
*/
FRAME(disable_int7,sp,0,ra)
	.set noreorder

	la	t0,IRLVL4	# Set interrupt controller level (bit 8-10 = 0 to diasble int 7)
	li	t1,0x200
	sw	t1,(t0)

	j	ra
	nop
	.set reorder
ENDFRAME(disable_int7)
#endif

/*************************************************************
*  exception:
*       Diagnostic code that can be hooked to PMON interrupt handler.
*       Generates pulse on PIO22 pin.
*	Called from _exception code in PMON (see mips.s of PMON).
*	Return address is located in k1.
*/
FRAME(tx49xxexception,sp,0,ra)
	.set noreorder
	la	k0,k1tmp
	sw	k1,(k0)

/* Activate TX49xx PIO22 signal for diagnostics */
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	lw	k0,(k0)
	lui	k1,0x40
	or	k1,k1,k0
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	sw	k1,(k0)
	nop

/* De-activate TX49xx PIO22 signal for diagnostics */
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	lw	k0,(k0)
	lui	k1,0x40
	not	k1
	and	k1,k1,k0
	lui	k0,0xff1f
	ori	k0,k0,0xf500
	sw	k1,(k0)
	nop

	la	k0,k1tmp
	lw	k1,(k0)
	j	k1
	.set reorder
ENDFRAME(tx49xxexception)




	.data

k1tmp:	.word	0	/* Temporary strage for K1 during interrupt service */

/*************************************************************
*
* Exception handler links, used in PMON exception handler chains
*/
	/* Interrupt exception service routine link */
	.global	_int_esr_link
_int_esr_link:
	.word	0
	.word	bsp_ISR_Handler

	/* Break exception service routine link */
	.global	_brk_esr_link
_brk_esr_link:
	.word	0
	.word	_BRK_Handler




