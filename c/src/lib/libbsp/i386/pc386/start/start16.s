/*-------------------------------------------------------------------------+
| start16hi.s v1.0 - PC386 BSP - 1998/04/13
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
| This code is partly based on (from the Linux source tree):
|   video.S - Copyright (C) 1995, 1996 Martin Mares <mj@k332.feld.cvut.cz>
+--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------+
| Constants
+----------------------------------------------------------------------------*/

.set PROT_CODE_SEG, 0x08	# offset of code segment descriptor into GDT
.set CR0_PE,        1		# protected mode flag on CR0 register
.set START32,       START32ADDR # address of protected mode code

/*----------------------------------------------------------------------------+
| A Descriptor table register has the following format:	
+----------------------------------------------------------------------------*/

.set DTR_LIMIT, 0		# offset of two byte limit
.set DTR_BASE,  2		# offset of four byte base address
.set DTR_SIZE,  6		# size of DTR register

/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

.text

	.globl _start16		# entry point
	.globl start16
start16:
_start16:

.code16

#if defined(RTEMS_VIDEO_80x50)
	
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

#endif /* RTEMS_VIDEO_80x50 */

        cli			# DISABLE INTERRUPTS!!!

	/*---------------------------------------------------------------------+
	| Bare PC machines boot in real mode! We have to turn protected mode on.
	+---------------------------------------------------------------------*/

	movl	$gdtptr, %eax
	andl	$0x0000ffff, %eax	# get offset into segment
	lgdt	%cs:(%eax)		# load Global Descriptor Table
	movl	$idtptr, %eax
	andl	$0x0000ffff, %eax	# get offset into segment
	lidt	%cs:(%eax)		# load Interrupt Descriptor Table
	
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0		# turn on protected mode

	ljmp	$PROT_CODE_SEG, $1f	# flush prefetch queue
1:

.code32

        /*---------------------------------------------------------------------+        | we have to enable A20 in order to access memory above 1MByte
        +---------------------------------------------------------------------*/
	call	empty_8042
	movb	$0xD1, %al		# command write
	outb	%al, $0x64
	call	empty_8042
	movb	$0xDF, %al		# A20 on
	outb	%al, $0x60
	call	empty_8042

	ljmp	$PROT_CODE_SEG, $START32	# jump to start of 32 bit code

/*----------------------------------------------------------------------------+
| delay
+------------------------------------------------------------------------------
| Delay is needed after doing I/O. We do it by writing to a non-existent port.
+----------------------------------------------------------------------------*/
	.globl _delay
	.globl delay
delay:
_delay:
	outb	%al, $0xED	# about 1uS delay
	ret

/*----------------------------------------------------------------------------+
| empty_8042
+------------------------------------------------------------------------------
| This routine checks that the keyboard command queue is empty (after emptying
| the output buffers).
| No timeout is used - if this hangs there is something wrong with the machine,
| and we probably couldn't proceed anyway.
+----------------------------------------------------------------------------*/
	.globl _empty_8042
	.globl empty_8042
empty_8042:
_empty_8042:
	call	delay
	inb	$0x64, %al	# 8042 status port
	testb	$0x01, %al	# output buffer?
	jz	no_output
	call	delay
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

	.align	4
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
	
/*************************************
* INTERRUPT DESCRIPTOR TABLE POINTER *
*************************************/

	.align	4
idtptr:
	.word	0x07ff	# limit at maximum (allows all 256 interrupts)
	.word	0, 0	# base at 0
