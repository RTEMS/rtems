/*-------------------------------------------------------------------------+
| ldsegs.s v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file assists the board independent startup code by loading the proper
| segment register values. The values loaded are board dependent. In addition
| it contains code to enable the A20 line and to reprogram the PIC to relocate
| the IRQ interrupt vectors to 0x20 -> 0x2f.
| NOTE: No stack has been established when this routine is invoked.
|	It returns by jumping back to bspentry.
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
| This code is base on:
|   ldsegs.s,v 1.4 1996/04/20 16:48:30 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
|
| Also based on (from the Linux source tree):
|   setup.S - Copyright (C) 1991, 1992 Linus Torvalds
+--------------------------------------------------------------------------*/


#include "asm.h"

/*----------------------------------------------------------------------------+
| Constants
+----------------------------------------------------------------------------*/
.set	PROT_DATA_SEG, 0x10	# offset in gdt
.set	RESET_SS, PROT_DATA_SEG	# initial value of stack segment register
.set	RESET_DS, PROT_DATA_SEG	# initial value of data segment register
.set	RESET_ES, PROT_DATA_SEG	# initial value of extra segment register
.set	RESET_FS, PROT_DATA_SEG	# initial value of "f" segment register
.set	RESET_GS, PROT_DATA_SEG	# initial value of "g" segment register


/*----------------------------------------------------------------------------+
| Macros
+----------------------------------------------------------------------------*/
#define LOAD_SEGMENTS(_value, _segment) \
	movw	$ ## _value, ax; \
	movw	ax, _segment

/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

BEGIN_CODE

        EXTERN (establish_stack)

/*----------------------------------------------------------------------------+
| empty_8042
+------------------------------------------------------------------------------
| This routine checks that the keyboard command queue is empty (after emptying
| the output buffers).
| No timeout is used - if this hangs there is something wrong with the machine,
| and we probably couldn't proceed anyway.
+----------------------------------------------------------------------------*/
SYM(empty_8042):
	call	delay
	inb	$0x64, al	# 8042 status port
	testb	$0x01, al	# output buffer?
	jz	SYM(no_output)
	call	SYM(delay)
	in	$0x60, al	# read it
	jmp	SYM(empty_8042)
SYM(no_output):
	test	$0x02, al	# is input buffer full?
	jnz	SYM(empty_8042)	# yes - loop
	ret

/*----------------------------------------------------------------------------+
| delay
+------------------------------------------------------------------------------
| Delay is needed after doing I/O. We do it by writing to a non-existent port.
+----------------------------------------------------------------------------*/
SYM(delay):
	outb	al, $0xED	# about 1uS delay
	ret

/*-------------------------------------------------------------------------+
|         Function: _load_segments
|      Description: Load board segment registers with apropriate values + enable
	            A20 line + reprogram PIC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
        PUBLIC (_load_segments)
SYM (_load_segments):
	
	LOAD_SEGMENTS(RESET_SS, ss)
	LOAD_SEGMENTS(RESET_DS, ds)
	LOAD_SEGMENTS(RESET_ES, es)
	LOAD_SEGMENTS(RESET_FS, fs)
	LOAD_SEGMENTS(RESET_GS, gs)

	/*---------------------------------------------------------------------+
	| we have to enable A20 in order to access memory above 1MByte
	+---------------------------------------------------------------------*/

	call	SYM(empty_8042)
	movb	$0xD1, al		# command write
	outb	al, $0x64
	call	SYM(empty_8042)
        movb	$0xDF, al		# A20 on
	outb	al, $0x60
	call	SYM(empty_8042)

	/*---------------------------------------------------------------------+
	| Now we have to reprogram the interrupts :-(. We put them right after
	| the intel-reserved hardware interrupts, at int 0x20-0x2F. There they
	| won't mess up anything. Sadly IBM really messed this up with the
	| original PC, and they haven't been able to rectify it afterwards. Thus
	| the bios puts interrupts at 0x08-0x0f, which is used for the internal
	| hardware interrupts as well. We just have to reprogram the 8259's, and
	| it isn't fun.
	+---------------------------------------------------------------------*/

	movb	$0x11, al		/* initialization sequence          */
	outb	al, $0x20		/* send it to 8259A-1               */
	call	SYM(delay)
	outb	al, $0xA0		/* and to 8259A-2                   */
	call	SYM(delay)
	
	movb	$0x20, al		/* start of hardware int's (0x20)   */
	outb	al, $0x21
	call	SYM(delay)
	movb	$0x28, al		/* start of hardware int's 2 (0x28) */
	outb	al, $0xA1
	call	SYM(delay)
	
	movb	$0x04, al		/* 8259-1 is master                 */
	outb	al, $0x21
	call	SYM(delay)
	movb	$0x02, al		/* 8259-2 is slave                  */
	outb	al, $0xA1
	call	SYM(delay)
	
	movb	$0x01, al		/* 8086 mode for both               */
	outb	al, $0x21
	call	SYM(delay)
	outb	al, $0xA1
	call	SYM(delay)
	
	movb	$0xFF, al		/* mask off all interrupts for now  */
	outb	al, $0xA1
	call	SYM(delay)
	movb	$0xFB, al		/* mask all irq's but irq2 which    */
	outb	al, $0x21		/* is cascaded                      */
	call	SYM(delay)

	jmp	SYM (_establish_stack)	# return to the bsp entry code

/*-------------------------------------------------------------------------+
|         Function: _return_to_monitor
|      Description: Return to board's monitor (we have none so simply restart).
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
        PUBLIC (_return_to_monitor)
SYM (_return_to_monitor):

	call	SYM (Timer_exit)
	call	SYM (Clock_exit)
	jmp	SYM (start)

END_CODE

/*----------------------------------------------------------------------------+
| DATA section
+----------------------------------------------------------------------------*/

BEGIN_DATA

        PUBLIC (_Do_Load_IDT)
SYM (_Do_Load_IDT):
       .byte 1	# load RTEMS own Interrupt Descriptor Table

        PUBLIC (_Do_Load_GDT)
SYM (_Do_Load_GDT):
       .byte 0	# use the Global Descriptor Table that is already defined

END_DATA

END
