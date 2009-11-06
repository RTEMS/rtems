/*-------------------------------------------------------------------------+
| start.s v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the entry point for the application.
| The name of this entry point is compiler dependent.
| It jumps to the BSP which is responsible for performing all initialization.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Modified the 20/05/1998  by valette@crf.canon.fr in order to give a working
| example of eraly stage debugging via the DEBUG_EARLY_START define.
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on an earlier generation RTEMS i386 start.s and the
| following copyright applies:
|
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

/*
 * The most trivial start.s possible. It does not know anything
 * about system it is running on, so it will jump to appropriate
 * place in BSP specific place to do things it knows nothing about
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

/*----------------------------------------------------------------------------+
| Size of heap and stack:
+----------------------------------------------------------------------------*/

#ifndef CPU_STACK_ALIGNMENT
#error  "Missing header ? CPU_STACK_ALIGNMENT NOT DEFINED"
#endif

.set STACK_SIZE, 0x1000

/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

BEGIN_CODE

	PUBLIC (start)		# GNU default entry point

	EXTERN (boot_card)
	EXTERN (_load_segments)
	EXTERN (_return_to_monitor)
	EXTERN (_IBMPC_initVideo)
	EXTERN (debugPollingGetChar)
	EXTERN (checkCPUtypeSetCr0)
	EXTERN (BSP_console_select)

/*
 * In case this crashes on your machine and this is not due
 * to video mode set by the loader, you may try to define
 * the following variable:
 */
/* #define DEBUG_EARLY_START */

SYM (start):
        /*
         *  When things are really, REALLY!, bad -- turn on the speaker and
         *  lock up.  This shows whether or not we make it to a certain
         *  location.
         */
#if 0
        inb     $0x61, al
        orb     $0x03, al
        outb    al, $0x61       # enable the speaker
speakl:	jmp	speakl             # and SPIN!!!
#endif

        nop
        cli			# DISABLE INTERRUPTS!!!
	cld

	/* Save multiboot info if we detect a multiboot loader */
	cmp 	$0x2badb002,eax
	jne	2f
	
	/* We have multiboot info; let's hope DS and ES are OK... */
	movl	ebx, SYM(_boot_multiboot_info_p)
	/* Check for memory size info and save */
	movl	ebx, esi
	movl	(esi), eax
	movl	eax, ebx
	movl	$SYM(_boot_multiboot_info), edi
 	/* save flags, always present */
	movsd
	/* flag 1 is memory */
	and	$1, eax
	je	1f
	movl	$2, ecx
	rep movsd
	/* flag 2 is the command line */
1:	movl	ebx, eax
	and	$4, eax
	je	3f
	movl    (_boot_multiboot_info_p), eax
	movl    16(eax), esi
	movl	$255, ecx
2:	movzbl	(esi), eax
	test	al, al
	je	3f
	movb	al, (edi)
	inc	edi
	inc	esi
	dec	ecx
	je	3f
	jmp	2b
3:	xor	al, al
	movb	al, (edi)
#ifdef DEBUG_EARLY_START
	/*
	 * Must get video attribute to have a working printk.
	 * Note that the following code assume we already have
	 * valid segments and a stack. It should be true for
	 * any loader starting RTEMS in protected mode (or
	 * at least I hope so :	-)).
	 */
	call _IBMPC_initVideo
	/*
	 * try printk and a getchar in polling mode ASAP
	 */
	pushl	$welcome_msg
	call	printk
	addl	$4, esp

	/* call	debugPollingGetChar */

#endif

/*----------------------------------------------------------------------------+
| Load the segment registers (this is done by the board's BSP) and perform any
| other board specific initialization procedures, this piece of code
| does not know anything about
|
| NOTE: Upon return, gs will contain the segment descriptor for a segment which
|       maps directly to all of physical memory.
+----------------------------------------------------------------------------*/

	jmp	SYM (_load_segments)	# load board dependent segments

/*----------------------------------------------------------------------------+
| Set up the stack
+----------------------------------------------------------------------------*/

	PUBLIC (_establish_stack)
SYM (_establish_stack):

	movl	$_end, eax		# eax = end of bss/start of heap
	addl	$STACK_SIZE, eax	# make room for stack
	subl    $4,          eax    # reserve room for arg to 'boot_card'
	andl	$ - CPU_STACK_ALIGNMENT, eax	# align SP on CPU_STACK_ALIGNMENT boundary
	movl	eax, esp		# set stack pointer
	movl	eax, ebp		# set base pointer

/*----------------------------------------------------------------------------+
| Zero out the BSS segment
+----------------------------------------------------------------------------*/

SYM (zero_bss):
	cld				# make direction flag count up
	movl	$ SYM (_end), ecx	# find end of .bss
	movl	$ SYM (__bss_start), edi # edi = beginning of .bss
	subl	edi, ecx		# ecx = size of .bss in bytes
	shrl	ecx			# size of .bss in longs
	shrl	ecx
	xorl	eax, eax		# value to clear out memory
	repne				# while ecx != 0
	stosl				#   clear a long in the bss

/*-------------------------------------------------------------------+
| Initialize the video because zero_bss has cleared initVideo parameters
| if it was called earlier
| So from now we can use printk
+-------------------------------------------------------------------*/
	call _IBMPC_initVideo

/*---------------------------------------------------------------------+
| Check CPU type. Enable Cache and init coprocessor if needed.
+---------------------------------------------------------------------*/
	call checkCPUtypeSetCr0

/*---------------------------------------------------------------------+
| Redirect printk (and console) port if desired
+---------------------------------------------------------------------*/
	call SYM(BSP_console_select)

/*---------------------------------------------------------------------+
| Transfer control to User's Board Support Package
| Note: at the top we reserved space for the argument
|       so that
|          initial_esp = ( TOS - 4 ) & ~(CPU_STACK_ALIGNMENT-1)
|       this ensures that
|       1) esp is now aligned
|       2) there is space for the cmdline pointer which we just
|          may store at *(esp)
+---------------------------------------------------------------------*/

	movl	$SYM(_boot_multiboot_cmdline), (esp)
	call	SYM (boot_card)

/*---------------------------------------------------------------------+
| Clean up - we do not know anything about it, so we will
| jump to BSP specific code to do cleanup
+---------------------------------------------------------------------*/

	jmp	SYM (_return_to_monitor)

END_CODE

BEGIN_DATA
	PUBLIC(_boot_multiboot_info_p)
SYM(_boot_multiboot_info_p):
	.long 0

	PUBLIC(_boot_multiboot_info)
	PUBLIC(_boot_multiboot_flags)
	PUBLIC(_boot_multiboot_memory)
	PUBLIC(_boot_multiboot_cmdline)
SYM(_boot_multiboot_info):
SYM(_boot_multiboot_flags):
	.long 0	/* flags */
SYM(_boot_multiboot_memory):
	.long 0 /* mem_lower */
	.long 0 /* mem_upper */
SYM(_boot_multiboot_cmdline):
	.rept 256 /* cmd line */
	.byte 0
	.endr
	
	PUBLIC(_stack_size)
SYM(_stack_size):
	.long STACK_SIZE

#ifdef DEBUG_EARLY_START

	PUBLIC (welcome_msg)
SYM (welcome_msg) :
	.string "Ready to debug RTEMS ?\nEnter <CR>\n"

	PUBLIC (hex_msg)
SYM (hex_msg) :
	.string "0x%x\n"

	PUBLIC (made_it_msg)
SYM (made_it_msg) :
	.string "made it to %d\n"

#endif

END_DATA

END
