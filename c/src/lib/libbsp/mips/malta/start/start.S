/*
 * start.S -- startup file for JMR3904 BSP based upon crt0.S from
 * newlib-1.8.2/libgloss/mips and adapted for RTEMS.
 *
 * crt0.S -- startup file for MIPS.
 *
 * Copyright (c) 1995, 1996, 1997 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <rtems/asm.h>
#include <bsp/regs.h>

#ifdef __mips16
/* This file contains 32 bit assembly code.  */
	.set nomips16
#endif

/* This is for referencing addresses that are not in the .sdata or
   .sbss section under embedded-pic, or before we've set up gp.  */
#ifdef __mips_embedded_pic
# ifdef __mips64
#  define LA(t,x) la t,x-PICBASE ; daddu t,s0,t
# else
#  define LA(t,x) la t,x-PICBASE ; addu t,s0,t
# endif
#else /* __mips_embedded_pic */
# define LA(t,x) la t,x
#endif /* __mips_embedded_pic */

	.text
	.align	2

/* Without the following nop, GDB thinks _start is a data variable.
 * This is probably a bug in GDB in handling a symbol that is at the
 * start of the .text section.
 */
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	.globl	_start
	.ent	_start
_start:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	.set	noreorder
	/* Get the address of start into $5 in a position independent fashion.
	** This lets us know whether we have been relocated or not.
	*/
	$LF1 = . + 8
	bal     $LF1
	nop
_branch:
#if 0
	move	$5, $31				# $5 == where are we
	li	$6, 0x8800000c			# $6 == where we want to be
/*	#la      $6,_branch */
	beq	$5, $6, _start_in_ram
	nop
	/* relocate the code from EEPROM to RAM */
        la	$7, _edata
relocate:
        nop
	lw	$8, ($5)			# $8 = *EEPROM
	addu	$5, $5, 4			# EEPROM++
        sw      $8, ($6)			# *RAM = $8
	addu	$6, $6, 4			# RAM++
	bne	$6, $7, relocate		# copied all the way to edata?
	nop
	la	$6, _start_in_ram
	jr	$6
	nop
	.end _start

	.globl	_start_in_ram
	.ent _start_in_ram
#endif
_start_in_ram:
	nop
#if 0
#ifdef __mips_embedded_pic
	PICBASE = .+8
        bal	PICBASE
	nop
	move	s0,$31
#endif
#endif
	li	v0, SR_CU1|SR_PE|SR_FR|SR_KX|SR_SX|SR_UX
	mtc0	v0, C0_SR
	mtc0	zero, C0_CAUSE

#if 0
/* Check for FPU presence */
#ifndef __mips_soft_float
/* This doesn't work if there is no FPU.  We get illegal instruction
   exceptions.  */
	li	t2,0xAAAA5555
	mtc1	t2,fp0		/* write to FPR 0 */
	mtc1	zero,fp1	/* write to FPR 1 */
	mfc1	t0,fp0
	mfc1	t1,fp1
	nop
	bne	t0,t2,1f	/* check for match */
	nop
	bne	t1,zero,1f	/* double check */
	nop
#ifndef __mips64  /* Clear the FR bit */
	li	v0, SR_CU1|SR_PE|SR_KX|SR_SX|SR_UX
	mtc0	v0, C0_SR
#endif
	j	2f
	nop
#endif
#endif

1:
	li	v0, SR_PE|SR_FR|SR_KX|SR_SX|SR_UX
	mtc0	v0, C0_SR
2:
/* Fix high bits, if any, of the PC so that exception handling
   doesn't get confused.  */
	LA (v0, 3f)
	jr	v0
	nop
3:
	LA (gp, _gp)				# set the global data pointer
#if 0
	.end _start_in_ram
#else
	.end _start
#endif

/*
 * zero out the bss section.
 */
	.globl	__memsize
	.globl	zerobss
	.ent	zerobss
zerobss:
	LA (v0, _fbss)
	LA (v1, _end)
3:
	sw	zero,0(v0)
	bltu	v0,v1,3b
	addiu	v0,v0,4				# executed in delay slot

	la	t0, _stack_init			# initialize stack so we
	/* We must subtract 24 bytes for the 3 8 byte arguments to main, in
	   case main wants to write them back to the stack.  The caller is
	   supposed to allocate stack space for parameters in registers in
	   the old MIPS ABIs.  We must do this even though we aren't passing
	   arguments, because main might be declared to have them.

	   Some ports need a larger alignment for the stack, so we subtract
	   32, which satisifes the stack for the arguments and keeps the
	   stack pointer better aligned.  */
	subu	t0,t0,32
	move	sp,t0				# set stack pointer
	.end	zerobss

	.globl	exit .text
	.globl	init
	.ent	init
init:
        nop
	jal	init_tlb		/* clear the tlb */
	move	a0,zero				# set command line to 0
	jal	boot_card			# call the program start function
	nop

dead:
	b	dead
	nop
	.end	init

/*
 * _sys_exit -- Exit from the application. Normally we cause a user trap
 *          to return to the ROM monitor for another run. NOTE: This is
 *	    the only other routine we provide in the crt0.o object, since
 *          it may be tied to the "_start" routine. It also allows
 *          executables that contain a complete world to be linked with
 *          just the crt0.o object.
 */
	.globl	_sys_exit
	.ent _sys_exit
_sys_exit:
7:
#ifdef GCRT0
	jal	_mcleanup
	nop
#endif
	/* break instruction can cope with 0xfffff, but GAS limits the range: */
	break	1023
	nop
	b	7b				# but loop back just in-case
	nop
	.end _sys_exit

/* EOF crt0.S */
