/*-------------------------------------------------------------------------+
| start16.s v1.0 - PC386 BSP - 1998/04/13
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
+--------------------------------------------------------------------------*/

#include <bspopts.h>

/*----------------------------------------------------------------------------+
| Constants
+----------------------------------------------------------------------------*/

.set PROT_CODE_SEG, 0x08	# offset of code segment descriptor into GDT
.set PROT_DATA_SEG, 0x10	# offset of code segment descriptor into GDT
.set CR0_PE,        1		# protected mode flag on CR0 register
.set HDRSTART,      HEADERADDR	# address of start of bin2boot header
.set HDROFF,        0x24	# offset into bin2boot header of start32 addr
.set STACKOFF,      0x200-0x10  # offset to load into %esp, from start of image

 /* #define NEW_GAS */
/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

.text

	.globl _start16		# entry point
	.globl start16
start16:
_start16:

.code16

	cli			# DISABLE INTERRUPTS!!!

	movw	%cs, %ax	#
	movw	%ax, %ds	# set the rest of real mode registers
	movw	%ax, %es	#
	movw	%ax, %ss	#

#if (RTEMS_VIDEO_80x50 == 1)

	movl	$0x0040,%eax	# use 32 bit constant to ensure 16 MSB=0
	mov     %ax,%es
	movw	%es:0x4a, %ax	# get 16 bit number of columns
	cmpw	$0, %ax         # or 0 if no video adapter
	je	1f              # if no video, skip touching it
	/*---------------------------------------------------------------------+
	| Switch VGA video to 80 lines x 50 columns mode. Has to be done before
	| turning protected mode on since it uses BIOS int 10h (video) services.
	+---------------------------------------------------------------------*/

	movw	$0x0003, %ax	# forced set
	int	$0x10
	movw	$0x1112, %ax	# use 8x8 font
	xorb	%bl, %bl
	int	$0x10
	movw	$0x1201, %ax	# turn off cursor emulation
	movb	$0x34, %bl
	int	$0x10
	movb	$0x01, %ah	# define cursor (scan lines 0 to 7)
	movw	$0x0007, %cx
	int	$0x10

1:
#endif /* RTEMS_VIDEO_80x50 */

	/*---------------------------------------------------------------------+
	| Bare PC machines boot in real mode! We have to turn protected mode on.
	+---------------------------------------------------------------------*/

	lgdt	gdtptr - start16	# load Global Descriptor Table
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0		# turn on protected mode

#ifdef NEW_GAS
	ljmpl	$PROT_CODE_SEG, $1f	# flush prefetch queue, and reload %cs
#else
	ljmp	$PROT_CODE_SEG, $1f	# flush prefetch queue, and reload %cs
#endif
.code32
1:
        /*---------------------------------------------------------------------+
        | load the other segment registers
        +---------------------------------------------------------------------*/
	movl	$PROT_DATA_SEG, %eax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movl	$start16 + STACKOFF, %esp	# set up stack pointer
	addl	$start16 + STACKOFF, %ebp	# set up stack pointer

        /*---------------------------------------------------------------------+
        | we have to enable A20 in order to access memory above 1MByte
        +---------------------------------------------------------------------*/
	call	empty_8042
	movb	$0xD1, %al		# command write
	outb	%al, $0x64
	call	empty_8042
	movb	$0xDF, %al		# A20 on
	outb	%al, $0x60
	call	empty_8042

	call	pc386_delay
	call	pc386_delay
	call	pc386_delay

	movl	%cs:HDRSTART + HDROFF, %eax	#
	pushl	%eax				# jump to start of 32 bit code
	ret					#

/*----------------------------------------------------------------------------+
| pc386_delay
+------------------------------------------------------------------------------
| Delay is needed after doing I/O.
|
| The outb version is OK on most machines BUT the loop version ...
|
| will delay for 1us on 1Gz machine, it will take a little bit
| longer on slower machines, however, it does not matter because we
| are going to call this function only a few times
!
| NOTE: Saving the content of the EAX register just in case. - Rosimildo.
+----------------------------------------------------------------------------*/
	.p2align 4
	.globl _pc386_delay
	.globl pc386_delay
pc386_delay:
_pc386_delay:
	pushl	%eax
#if defined(USE_OUTB_FOR_DELAY)
	outb	%al, $0x80	# about 1uS delay on most machines

#else

	movl	$0x200, %eax
pc386_delay1:
	dec	%eax
	jnz	pc386_delay1
#endif
	popl	%eax
 	ret

/*----------------------------------------------------------------------------+
| empty_8042
+------------------------------------------------------------------------------
| This routine checks that the keyboard command queue is empty (after emptying
| the output buffers).
| No timeout is used - if this hangs there is something wrong with the machine,
| and we probably couldn't proceed anyway.
+----------------------------------------------------------------------------*/
	.p2align 4
	.globl _empty_8042
	.globl empty_8042
empty_8042:
_empty_8042:
	call	pc386_delay
	inb	$0x64, %al	# 8042 status port
	testb	$0x01, %al	# output buffer?
	jz	no_output
	call	pc386_delay
	in	$0x60, %al	# read it
	jmp	empty_8042
no_output:
	test	$0x02, %al	# is input buffer full?
	jnz	empty_8042	# yes - loop
	ret

/*----------------------------------------------------------------------------+
| DATA section
+----------------------------------------------------------------------------*/

/**************************
* GLOBAL DESCRIPTOR TABLE *
**************************/

	.p2align 4
gdtptr:
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
