/*
 * start.S -- startup file for Cogent CSB350 Au1100 based board
 *
 *  Copyright (c) 2005 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems/asm.h>
#include <bsp/regs.h>

	.text
	.align	2

/* Without the following nop, GDB thinks _start is a data variable.
 * This is probably a bug in GDB in handling a symbol that is at the
 * start of the .text section.
 */
	nop

	.globl	_start
	.ent	_start
_start:
	.set	noreorder

	/* Get the address of start into $5 in a position independent
	 * fashion. This lets us know whether we have been relocated or not.
	 */
	$LF1 = . + 8
	bal     $LF1
	nop
_branch:
	move	$5, $31			/* $5 == where are we */
	li	$6, 0x8800000c		/* $6 == where we want to be */

	li	v0, SR_CU1|SR_PE|SR_FR|SR_KX|SR_SX|SR_UX
	mtc0	v0, C0_SR
	mtc0	zero, C0_CAUSE

1:
	li	v0, SR_PE|SR_FR|SR_KX|SR_SX|SR_UX
	mtc0	v0, C0_SR
2:
/* Fix high bits, if any, of the PC so that exception handling
   doesn't get confused.  */
	la v0, 3f
	jr	v0
	nop
3:
	la gp, _gp			/* set the global data pointer */
	.end _start

/*
 * zero out the bss section.
 */
	.globl	__memsize
	.globl	zerobss
	.ent	zerobss
zerobss:
	la v0, _fbss
	la v1, _end
3:
	sw	zero,0(v0)
	bltu	v0,v1,3b
	addiu	v0,v0,4			/* executed in delay slot */

	la	t0, _stack_init		/* initialize stack so we */
	/* We must subtract 24 bytes for the 3 8 byte arguments to main, in
	   case main wants to write them back to the stack.  The caller is
	   supposed to allocate stack space for parameters in registers in
	   the old MIPS ABIs.  We must do this even though we aren't passing
	   arguments, because main might be declared to have them.

	   Some ports need a larger alignment for the stack, so we subtract
	   32, which satisifes the stack for the arguments and keeps the
	   stack pointer better aligned.  */
	subu	t0,t0,32
	move	sp,t0			/* set stack pointer */
	.end	zerobss

	.globl	exit .text
	.globl	init
	.ent	init
init:

	move	a0,zero			/* set command line to 0 */
	jal	boot_card		/* call the program start function */
	nop

	/* fall through to the "exit" routine */
	jal	_sys_exit		/* call libc exit to run the G++ */
					/* destructors */
	move	a0,v0			/* pass through the exit code */
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
	/* break inst. can cope with 0xfffff, but GAS limits the range: */
	break	1023
	nop
	b	7b			/* but loop back just in-case */
	nop
	.end _sys_exit

/* EOF crt0.S */
