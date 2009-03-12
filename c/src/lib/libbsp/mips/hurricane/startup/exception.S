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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
/* @(#)exception.S       10/11/04     1.00 */

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <usc.h>


#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  name;                           \
        .ent    name;                           \
name:;                                          \
        .frame  frm_reg,offset,ret_reg
#define ENDFRAME(name)                          \
        .end name


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


EXTERN(_ISR_Nest_level, 4)
EXTERN(_Thread_Dispatch_disable_level,4)
EXTERN(_Context_Switch_necessary,1)
EXTERN(_ISR_Signals_to_thread_executing,1)
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
FRAME(hurricane_ISR_Handler,sp,0,ra)
	.set noreorder

	mfc0 k0,C0_CAUSE	/* Determine if an interrupt generated this exception */
	nop
	and k1,k0,CAUSE_EXCMASK
	beq k1,zero,_chk_int	/* If so, branch to service here */
	nop
1:	la  k0,_int_esr_link	/* Otherwise, jump to next exception handler in PMON exception chain */
	lw  k0,(k0)
	lw  k0,4(k0)
	j   k0
	nop
_chk_int:
	mfc0 k1,C0_SR
	nop
	and k0,k1
	and k0,CAUSE_IPMASK
	beq k0,zero,_ISR_Handler_quick_exit	/* external interrupt not enabled, ignore */

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
        lw	t0,_ISR_Nest_level
	NOP
        add	t0,t0,1
        sw	t0,_ISR_Nest_level
  /*
   *  _Thread_Dispatch_disable_level++;
   */
        lw	t1,_Thread_Dispatch_disable_level
	NOP
        add	t1,t1,1
        sw	t1,_Thread_Dispatch_disable_level

  /*
   *  Call the CPU model or BSP specific routine to decode the
   *  interrupt source and actually vector to device ISR handlers.
   */
	move	 a0,sp
        jal      mips_vector_isr_handlers
        NOP

_ISR_Handler_cleanup:

  /*
   *  --_ISR_Nest_level;
   */
        lw	t2,_ISR_Nest_level
	NOP
        add	t2,t2,-1
        sw	t2,_ISR_Nest_level
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
   *  if ( !_Context_Switch_necessary && !_ISR_Signals_to_thread_executing )
   *    goto the label "exit interrupt (simple case)"
   */
        lb	t0,_Context_Switch_necessary
        lb	t1,_ISR_Signals_to_thread_executing
	NOP
        or 	t0,t0,t1
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

#if __mips == 1
	/* disabled 7/29, gregm, this tasks context was saved previously in an interrupt,
	** so we'll just restore the task's previous interrupt enables.

	**
	** make sure previous int enable is on  because we're returning from an interrupt
	** which means interrupts have to be enabled
	
	li	t1,SR_IEP
	or	t0,t1
	*/
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

       .set    reorder

ENDFRAME(hurricane_ISR_Handler)


FRAME(_BRK_Handler,sp,0,ra)
	.set noreorder

	la	k0,INT_CFG3	/* Disable heartbeat interrupt in USC320, it interferes with PMON exception handler */
	lw	k1,(k0)
	li	k0,~HBI_MASK
	and	k1,k1,k0
	la	k0,INT_CFG3
	sw	k1,(k0)
	
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



/***************************************************************************
**
**   The following code was added to support boards using V3 USC320
**     system controller chip.
**
****************************************************************************/

/*************************************************************
*  init_hbt()
*	Initialize the heartbeat timer
*/
FRAME(init_hbt,sp,0,ra)
	.set noreorder
	la	t0,SYSTEM	# Unlock USC registers
	li	t1,0xA5
	sb	t1,(t0)
	
	la	t0,WD_HBI	# Initialize heatbeat and watchdog timers

				# (1 / 64 MHz) * 4000 * (63 + 1) = 4000.0 microseconds 
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 4000 * 5 = 20000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00003F00 | 0x5)

				# (1 / 64 MHz) * 4000 * (15 + 1) = 1000.0 microseconds 
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 1000 * 20 = 20000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00000F00 | 0x14)

				# (1 / 64 MHz) * 40000 * (15 + 1) = 10000.0 microseconds 
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 10000 * 20 = 200000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00009600 | 0x14)

	sw	t1,(t0)
	
	la	t0,SYSTEM	# Lock USC registers
	li	t1,0x60
	sb	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(init_hbt)

/*************************************************************
*  reset_wdt()
*	Reset the watchdog timer
*/
FRAME(reset_wdt,sp,0,ra)
	.set noreorder

	la	t0,WD_HBI+2	# Load address watchdog timer reset byte
	li	t1,WD_INIT	
	sb	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(reset_wdt)

/*************************************************************
*  disable_wdt()
*	Disable watchdog timer
*/
FRAME(disable_wdt,sp,0,ra)
	.set noreorder
	la	t0,WD_HBI	# Clear watchdog enable bit in control register
	lw	t1,(t0)
	li	t2,~WD_EN
	and	t1,t1,t2
	sw	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_wdt)

/*************************************************************
*  enable_hbi(ints)
*	Enable the heartbeat interrupt
*/
FRAME(enable_hbi,sp,0,ra)
	.set noreorder
	
	la	t0,INT_CFG3	# Enable heartbeat interrupt in USC320
	lw	t1,(t0)
	li	t2,(HBI_MASK | MODE_TOTEM_POLE)
	or	t1,t1,t2
	sw	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(enable_hbi)

/*************************************************************
*  disable_hbi(ints)
*	Disable the heartbeat interrupt
*/
FRAME(disable_hbi,sp,0,ra)
	.set noreorder
	la	t0,INT_CFG3	# Disable heartbeat interrupt in USC320
	lw	t1,(t0)
	li	t2,~HBI_MASK
	and	t1,t1,t2
	sw	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_hbi)


/*************************************************************
*  enable_wdi()
*	Enable the watchdog interrupt
*/
FRAME(enable_wdi,sp,0,ra)
	.set noreorder
	
	la	t0,INT_CFG1	# Enable watchdog interrupt in USC320
	lw	t1,(t0)
	li	t2,(WDI_MASK | MODE_TOTEM_POLE)
	or	t1,t1,t2
	sw	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(enable_wdi)

/*************************************************************
*  disable_wdi(ints)
*	Disable the watchdog interrupt
*/
FRAME(disable_wdi,sp,0,ra)
	.set noreorder
	
	la	t0,INT_CFG1	# Disable watchdog interrupt in USC320
	lw	t1,(t0)
	li	t2,~(WDI_MASK | MODE_TOTEM_POLE)
	and	t1,t1,t2
	sw	t1,(t0)
	
	la	t0,INT_STAT	# Clear watchdog interrupt status bit
	li	t1,WDI_MASK
	sw	t1,(t0)
	
	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_wdi)


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
	.word	hurricane_ISR_Handler
	
	/* Break exception service routine link */
	.global	_brk_esr_link
_brk_esr_link:
	.word	0
	.word	_BRK_Handler


	

