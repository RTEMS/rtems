/*

Based upon IDT provided code with the following release:

This source code has been made available to you by IDT on an AS-IS
basis. Anyone receiving this source is licensed under IDT copyrights
to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or
without modifications.  No license under IDT patents or patent
applications is to be implied by the copyright license.

Any user of this software should understand that IDT cannot provide
technical support for this software and will not be responsible for
any consequences resulting from the use of this software.

Any person who transfers this source code or any derivative work must
include the IDT copyright notice, this paragraph, and the preceeding
two paragraphs in the transferred software.

COPYRIGHT IDT CORPORATION 1996
LICENSED MATERIAL - PROGRAM PROPERTY OF IDT

  $Id$
*/ 

/*************************************************************************
**
** Copyright 1991-95 Integrated Device Technology, Inc.
**	All Rights Reserved
**
** idt_csu.S -- IDT stand alone startup code
**
**************************************************************************/
#include <iregdef.h>
#include <idtcpu.h>
#include <asm.h>


.extern _fbss,4		  /* this is defined by the linker */
.extern end,4		  /* this is defined by the linker */

.lcomm sim_mem_cfg_struct,12

	.text


#define TMP_STKSIZE  1024 

/**************************************************************************
**
**  start - Typicl standalone start up code required for R3000/R4000
**
**
**	1)  Initialize the STATUS Register
**		a) Clear parity error bit
**		b) Set co_processor 1 usable bit ON
**		c) Clear all IntMask Enables
**		d) Set kernel/disabled mode
**	2)  Initialize Cause Register
**		a)  clear software interrupt bits       
**	3)  Determine FPU installed or not
**		if not, clear CoProcessor 1 usable bit
**	4)  Clear bss area
**	5)  MUST allocate temporary stack until memory size determined
**	    It MUST be uncached to prevent overwriting when caches are cleared
**	6)  Install exception handlers
**	7)  Determine memory and cache sizes
**	8)  Establish permanent stack (cached or uncached as defined by bss)
**	9)  Flush Instruction and Data caches
** 	10)  If there is a Translation Lookaside Buffer, Clear the TLB
** 	11)  Execute initialization code if the IDT/c library is to be used
** 
** 	12)  Jump to user's "main()" (boot_card() for RTEMS)
** 	13)  Jump to promexit
**
**	IDT/C 5.x defines _R3000, IDT/C 6.x defines _R4000 internally. 
**	This is used to mark code specific to R3xxx or R4xxx processors.
**	IDT/C 6.x defines __mips to be the ISA level for which we're 
**	generating code. This is used to make sure the stack etc. is 
**	double word aligned, when using -mips3 (default) or -mips2, 
**	when compiling with IDT/C6.x
**
***************************************************************************/

FRAME(start,sp,0,ra)

	.set	noreorder
#ifdef _R3000
	li	v0,SR_PE|SR_CU1        /* reset parity error and set */
					/* cp1 usable */
#endif
#ifdef _R4000
#if __mips==3 || defined(R4650)
	li	v0,SR_CU1|SR_DE|SR_FR 	/* initally clear ERL, enable FPA 64bit regs*/
	                                /* 4650: Need fr to be set anyway */
#else
	li	v0,SR_CU1|SR_DE 	/* initally clear ERL, enable FPA 32bit regs*/
#endif /* mips3 */
#endif

	mtc0	v0,C0_SR		/* clr IntMsks/ kernel/disabled mode */
	nop
	mtc0	zero,C0_CAUSE		/* clear software interrupts */
	nop

#ifdef _R4000
	li	v0,CFG_C_NONCOHERENT	# initialise default cache mode
	mtc0	v0,C0_CONFIG
#endif

/*
**	check to see if an fpu is really plugged in 
*/
	li	t3,0xaaaa5555		/*  put a's and 5's in t3	*/
	mtc1	t3,fp0			/* try to write them into fp0   */	
	mtc1	zero,fp1		/* try to write zero in fp	*/
	mfc1	t0,fp0
	mfc1	t1,fp1
	nop
	bne	t0,t3,1f		/* branch if no match  */
	nop
	bne	t1,zero,1f		/* double check for positive id   */
	nop
	/* We have a FPU. clear fcsr */
	ctc1	zero, fcr31
	j	2f			/* status register already correct  */
	nop
1:
#ifdef _R3000
	li	v0, SR_PE		/* reset parity error/NO cp1 usable */
#endif

#ifdef _R4000
        li      v0,SR_DE 		/* clear ERL and disable FPA */
#endif

	mtc0	v0, C0_SR		/* reset status register */
2:
	la  	gp, _gp			

	la	v0,_fbss 		/* clear bss before using it */
	la	v1,end 			/* end of bss */
3:	sw	zero,0(v0)
	bltu	v0,v1,3b
	add	v0,4 


/************************************************************************
**
** 	Temporary Stack - needed to  handle stack saves until
**			  memory size is determined and permanent stack set
**
**			  MUST be uncached to avoid confusion at cache
**			       switching during memory sizing
**
*************************************************************************/
#if __mips==3
	/* For MIPS 3, we need to be sure that the stack is aligned on a
	 * double word boundary. 
	 */
	andi	t0, v0, 0x7
	beqz	t0, 11f   /* Last three bits Zero, already aligned */
	nop
	add	v0, 4
11:
#endif

	or	v0, K1BASE		/* switch to uncached */
	add	v1, v0, TMP_STKSIZE 	/* end of bss + length of tmp stack */
	sub	v1, v1, (4*4)		/* overhead */
	move	sp, v1			/* set sp to top of stack */
4:	sw	zero, 0(v0)  
	bltu	v0, v1, 4b		/* clear out temp stack */
	add	v0, 4 
	
	jal	mips_install_isr_entries/* install exception handlers */ 
	nop				/* MUST do before memory probes */

	la	v0, 5f
	li	v1, K1BASE		/* force into uncached space */
	or	v0, v1			/* during memory/cache probes */
	j	v0
	nop
5:
	la	a0, sim_mem_cfg_struct
	jal	sim_mem_cfg		/* Make SIM call to get mem size */
	nop
	la	a0, sim_mem_cfg_struct
	lw	a0, 0(a0)		/* Get memory size from struct */
#ifdef _R3000
	jal	config_Icache
	nop
	jal	config_Dcache		/* determine size of D & I caches */
	nop	
#endif
#ifdef _R4000
	jal	config_cache		/* determine size of D & I caches */
	nop
#endif

	move	v0, a0			/* mem_size */

#if __mips==3
	/* For MIPS 3, we need to be sure that the stack (and hence v0
	 * here) is aligned on a double word boundary. 
	 */
	andi	t0, v0, 0x7
	beqz	t0, 12f   /* Last three bits Zero, already aligned */
	nop
	subu	v0, 4	/* mem_size was not aligned on doubleword bdry????*/
12:
#endif


/*
 * P_STACKSIZE is the size of the Prom Stack.
 * the prom stack grows downward
 */
#define P_STACKSIZE     0x2000   /* sets stack size to 8k */


/**************************************************************************
**
**  Permanent Stack - now know top of memory, put permanent stack there	
**
***************************************************************************/ 

	la	t2, _fbss		/* cache mode as linked */
	and	t2, 0xF0000000		/* isolate segment */
	la	t1, 6f
	j	t1			/* back to original cache mode */
	nop
6:
	or 	v0, t2			/* stack back to original cache mode */
	addiu   v0,v0,-16		/* overhead */
	move	sp, v0			/* now replace count w top of memory */
	move	v1, v0
	subu	v1, P_STACKSIZE 	/* clear requested stack size */

7:	sw	zero, 0(v1)		/* clear P_STACKSIZE  stack */ 
	bltu	v1,v0,7b
	add	v1, 4 
	.set	reorder

#ifdef _R3000 
	jal	flush_Icache	
	jal	flush_Dcache		/* flush Data & Instruction caches */
#endif
#ifdef _R4000 
	jal	flush_cache_nowrite	/* flush Data & Instruction caches */
#endif



/**************************************************************************
**
**	If this chip supports a Translation Lookaside Buffer, clear it
**
***************************************************************************/ 

	.set	noreorder
	mfc0	t1,  C0_SR		/* look at Status Register */
	nop
	.set 	reorder
#ifdef _R3000
	li	t2, SR_TS		/* TLB Shutdown bit */
	and 	t1,t2      		/* TLB Shutdown if 1 */
	bnez	t1, 8f			/* skip clearing if no TLB */	
#endif

#ifndef R4650
	jal	init_tlb		/* clear the tlb */
#endif


/************************************************************************
**
**  Initialization required if using IDT/c or libc.a, standard C Lib 
**
**  can SKIP if not necessary for application
**
************************************************************************/
8:

	jal	idtsim_init_sbrk
	jal	idtsim_init_file
/***********************  END I/O initialization **********************/


	jal	boot_card

	jal	idtsim_promexit

ENDFRAME(start)


	.globl	sim_mem_cfg
sim_mem_cfg:
	.set noat
	.set noreorder
	li	AT, (0xbfc00000+((55)*8))
	jr	AT
	nop
	.set at
	.set reorder
