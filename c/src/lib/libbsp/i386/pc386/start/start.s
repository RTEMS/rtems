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
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on an earlier generation RTEMS i386 start.s and the
| following copyright applies:
|
| **************************************************************************
| *  COPYRIGHT (c) 1989-1997.
| *  On-Line Applications Research Corporation (OAR). 
| *  Copyright assigned to U.S. Government, 1994.
| *
| *  The license and distribution terms for this file may be
| *  found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
| Also based on (from the Linux source tree):
|   video.S - Copyright (C) 1995, 1996 Martin Mares <mj@k332.feld.cvut.cz>
|
|  $Id$
+--------------------------------------------------------------------------*/


#include "asm.h"

/*----------------------------------------------------------------------------+
| Constants
+----------------------------------------------------------------------------*/

#ifdef pc386

.set PROT_CODE_SEG, 0x08	# offset of code segment descriptor into GDT
.set CR0_PE,        1		# protected mode flag on CR0 register

#endif /* pc386 */

/*----------------------------------------------------------------------------+
| A Descriptor table register has the following format:	
+----------------------------------------------------------------------------*/

.set DTR_LIMIT, 0		# offset of two byte limit
.set DTR_BASE,  2		# offset of four byte base address
.set DTR_SIZE,  6		# size of DTR register

/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

BEGIN_CODE

	PUBLIC (start)		# GNU default entry point

	EXTERN (main)
	EXTERN (load_segments)
	EXTERN (exit)

SYM (start):

/*----------------------------------------------------------------------------+
| Switch VGA video to 80 lines x 50 columns mode. Has to be done before turning
| protected mode on since it uses BIOS int 10h (video) services.
+----------------------------------------------------------------------------*/

#if defined(pc386) && defined(RTEMS_VIDEO_80x50)
	
.code16

	movw	$0x0003, ax	# forced set
	int	$0x10
	movw	$0x1112, ax	# use 8x8 font
	xorb	%bl, %bl
	int	$0x10
	movw	$0x1201, ax	# turn off cursor emulation
	movb	$0x34, %bl
	int	$0x10
	movb	$0x01, ah	# define cursor (scan lines 0 to 7)
	movw	$0x0007, cx
	int	$0x10

.code32

#endif /* pc386 && RTEMS_VIDEO_80x50 */

        nop
        cli			# DISABLE INTERRUPTS!!!

/*----------------------------------------------------------------------------+
| Bare PC machines boot in real mode! We have to turn protected mode on.
+----------------------------------------------------------------------------*/

#ifdef pc386

	data16
	movl	$ SYM(gdtptr), eax
	data16
	andl	$0x0000ffff, eax	# get offset into segment
	addr16
	lgdt	cs:(eax)		# load Global Descriptor Table
	data16
	movl	$ SYM(idtptr), eax
	data16
	andl	$0x0000ffff, eax	# get offset into segment
	addr16
	lidt	cs:(eax)		# load Interrupt Descriptor Table
	
	movl	%cr0, eax
	data16
	orl	$CR0_PE, eax
	movl	eax, %cr0		# turn on protected mode

	data16
	ljmp	$PROT_CODE_SEG, $ SYM(next)	# flush prefetch queue

SYM(next):

#endif /* pc386 */

/*----------------------------------------------------------------------------+
| Load the segment registers (this is done by the board's BSP) and perform any
| other board specific initialization procedures. 
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
	addl	$heap_size, eax		# eax = end of heap
	movl	eax, stack_start	# Save for brk() routine
	addl	$stack_size, eax	# make room for stack
	andl	$0xffffffc0, eax	# align it on 16 byte boundary
	movl	eax, esp		# set stack pointer
	movl	eax, ebp		# set base pointer

/*----------------------------------------------------------------------------+
| Zero out the BSS segment
+----------------------------------------------------------------------------*/

SYM (zero_bss):
	cld				# make direction flag count up
	movl	$ SYM (_end), ecx	# find end of .bss
	movl	$ SYM (_bss_start), edi	# edi = beginning of .bss
	subl	edi, ecx		# ecx = size of .bss in bytes
	shrl	ecx			# size of .bss in longs
	shrl	ecx
	xorl	eax, eax		# value to clear out memory
	repne				# while ecx != 0
	stosl				#   clear a long in the bss

	/*---------------------------------------------------------------------+
	| Copy the Global Descriptor Table to our space
	+---------------------------------------------------------------------*/

	sgdt	SYM (_Original_GDTR)	# save original GDT
	movzwl	SYM (_Original_GDTR)+DTR_LIMIT, ecx	# size of GDT in bytes;
					# limit is 8192 entries * 8 bytes per

	/*---------------------------------------------------------------------+
	| make ds:esi point to the original GDT
	+---------------------------------------------------------------------*/

	movl	SYM (_Original_GDTR)+DTR_BASE, esi
	push	ds			# save ds
	movw	gs, ax
	movw	ax, ds

	/*---------------------------------------------------------------------+
	| make es:edi point to the new (our copy) GDT
	+---------------------------------------------------------------------*/

	movl	$ SYM (_Global_descriptor_table), edi

	rep
	movsb				# copy the GDT (ds:esi -> es:edi)

	pop	ds			# restore ds

	/*---------------------------------------------------------------------+
	| Build and load new contents of GDTR
	+---------------------------------------------------------------------*/

	movw	SYM (_Original_GDTR)+DTR_LIMIT, ecx	# set new limit
	movw	cx, SYM (_New_GDTR)+DTR_LIMIT

	push	$ SYM (_Global_descriptor_table)
	push	es
	call	SYM (i386_Logical_to_physical)
	addl	$6, esp
	movl	eax, SYM (_New_GDTR)+DTR_BASE	# set new base

	cmpb	$0, SYM (_Do_Load_GDT)	# Should the new GDT be loaded?
	je	SYM (no_gdt_load)	# NO, then branch
	lgdt	SYM (_New_GDTR)		# load the new GDT

SYM (no_gdt_load):

	/*---------------------------------------------------------------------+
	| Copy the Interrupt Descriptor Table to our space
	+---------------------------------------------------------------------*/

	sidt	SYM (_Original_IDTR)	# save original IDT
	movzwl	SYM (_Original_IDTR)+DTR_LIMIT, ecx	# size of IDT in bytes;
					#limit is 256 entries * 8 bytes per

	/*---------------------------------------------------------------------+
	| make ds:esi point to the original IDT
	+---------------------------------------------------------------------*/

	movl	SYM (_Original_IDTR)+DTR_BASE, esi

	push	ds			# save ds
	movw	gs, ax
	movw	ax, ds

	/*---------------------------------------------------------------------+
	| make es:edi point to the new (our copy) IDT
	+---------------------------------------------------------------------*/

	movl	$ SYM (Interrupt_descriptor_table), edi

	rep
	movsb				# copy the IDT (ds:esi -> es:edi)
	pop	ds			# restore ds

	/*---------------------------------------------------------------------+
	| Build and load new contents of IDTR
	+---------------------------------------------------------------------*/

	movw	SYM (_Original_IDTR+DTR_LIMIT), ecx	# set new limit
	movw	cx, SYM (_New_IDTR)+DTR_LIMIT

	push	$ SYM (Interrupt_descriptor_table)
	push	es
	call	SYM (i386_Logical_to_physical)
	addl	$6, esp
	movl	eax, SYM (_New_IDTR)+DTR_BASE		# set new base

	cmpb	$0, SYM (_Do_Load_IDT)	# Should the new IDT be loaded?
	je	SYM (no_idt_load)	# NO, then branch
	lidt	SYM (_New_IDTR)		# load the new IDT
SYM (no_idt_load):

	/*---------------------------------------------------------------------+
	| Initialize the i387.
	|
	| Using the NO WAIT form of the instruction insures that if it is not
	| present the board will not lock up or get an exception.
	+---------------------------------------------------------------------*/

	fninit				# MUST USE NO-WAIT FORM

	/*---------------------------------------------------------------------+
	| Transfer control to User's Board Support Package
	+---------------------------------------------------------------------*/

	pushl	$0			# environp
	pushl	$0			# argv
        pushl	$0			# argc
	call	SYM (main)
	addl	$12, esp

	/*---------------------------------------------------------------------+
	| Clean up
	+---------------------------------------------------------------------*/

	EXTERN (return_to_monitor)

	PUBLIC (Bsp_cleanup)

SYM (Bsp_cleanup):

	cmpb	$0, SYM (_Do_Load_IDT)	# Was the new IDT loaded?
	je	SYM (no_idt_restore)	# NO, then branch
	lidt	SYM (_Original_IDTR)	# restore the new IDT

SYM (no_idt_restore):

	cmpb	$0, SYM (_Do_Load_GDT)	# Was the new GDT loaded?
	je	SYM (no_gdt_restore)	# NO, then branch
	lgdt    SYM (_Original_GDTR)	# restore the new GDT

SYM (no_gdt_restore):

	jmp	SYM (_return_to_monitor)

END_CODE

/*----------------------------------------------------------------------------+
| DATA section
+----------------------------------------------------------------------------*/

BEGIN_DATA

#ifdef pc386

/**************************
* GLOBAL DESCRIPTOR TABLE *
**************************/

	.align	4
SYM(gdtptr):
	/* we use the NULL descriptor to store the GDT pointer - a trick quite
	   nifty due to: Robert Collins (rcollins@x86.org) */
	.word	gdtlen - 1
	.long	gdtptr
	.word   0x0000

	/* code segment */
	.word	0xffff, 0
	.byte	0, 0x9f, 0xcf, 0

        /* data segment */
	.word	0xffff, 0
	.byte	0, 0x93, 0xcf, 0

	.set	gdtlen, . - gdtptr	# length of GDT
	
/*************************************
* INTERRUPT DESCRIPTOR TABLE POINTER *
*************************************/

	.align	4
SYM(idtptr):
	.word	0x07ff	# limit at maximum (allows all 256 interrupts)
	.word	0, 0	# base at 0

#endif /* pc386 */

	EXTERN (Do_Load_IDT)	# defined in the BSP
	EXTERN (Do_Load_GDT)	# defined in the BSP

	.align	2
	PUBLIC (start_frame)
SYM (start_frame):
	.long	0

	PUBLIC (stack_start)
SYM (stack_start):
	.long	0

END_DATA

/*----------------------------------------------------------------------------+
| BSS section
+----------------------------------------------------------------------------*/

BEGIN_BSS

	PUBLIC (heap_size)
	.set	heap_size, 0x2000

	PUBLIC (stack_size)
	.set	stack_size, 0x1000

	PUBLIC (Interrupt_descriptor_table)
SYM (Interrupt_descriptor_table):
	.space (256 * 8)	# reserve space for all 256 interrupts

	PUBLIC (_Original_IDTR)
SYM (_Original_IDTR):
	.space DTR_SIZE

	PUBLIC (_New_IDTR)
SYM (_New_IDTR):
	.space DTR_SIZE

	PUBLIC (_Global_descriptor_table)
SYM (_Global_descriptor_table):
#ifdef pc386
	
	.space (3 * 8)	# the PC386 bsp only needs 3 segment descriptors:
#else			#   NULL, CODE and DATA
	.space (8192 * 8)
	
#endif /* pc386 */

	PUBLIC (_Original_GDTR)
SYM (_Original_GDTR):
	.space DTR_SIZE

	PUBLIC (_New_GDTR)
SYM (_New_GDTR):
	.space DTR_SIZE

	PUBLIC (_Physical_base_of_ds)
SYM (_Physical_base_of_ds):
	.space 4

	PUBLIC (_Physical_base_of_cs)
SYM (_Physical_base_of_cs):
	.space 4

END_BSS

END
