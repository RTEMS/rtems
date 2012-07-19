/*  dlentry.s
 *
 *  This file contains the entry code for RTEMS programs starting
 *  after download to RAM
 *
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP:
 *
 *  This file contains the entry veneer for RTEMS programs
 *  downloaded to Papyrus.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  derived from "helas403/dlentry.S":
 *
 *  Further changes to derive for the PPC405CR/GP/GPr/EX/EXr
 *  by Michael Hamel ADInstruments Ltd 2008
 */

#include <rtems/asm.h>

/*
 *  The virtex ELF link scripts support three special sections:
 *    .entry    The actual entry point
 *    .vectors  The section containing the interrupt entry veneers.
 */

/*
 *  Downloaded code loads the vectors separately to 0x00000100,
 *  so .entry can be over 256 bytes.
 *
 *  The other sections are linked in the following order:
 *    .entry
 *    .text
 *    .data
 *    .bss
 * see linker command file for section placement
 *
 *  The initial stack is set to stack.end
 *
 *  All the entry veneer has to do is to clear the BSS.
 */

/*
 *  GDB likes to have debugging information for the entry veneer.
 *  Here was some DWARF information. IMD removed it, because we
 *  could not check, whether it was still correct. Sorry.

 */


			.section .entry

			PUBLIC_VAR (start)
			PUBLIC_VAR (download_entry)
			PUBLIC_VAR (__rtems_entry_point)

SYM(start):
SYM(download_entry):
SYM(__rtems_entry_point):

			.extern SYM (boot_card)

			bl	.startup		/* First word is branch to reset_entry */


/*---------------------------------------------------------------------------
 * Parameters from linker
 *--------------------------------------------------------------------------*/

base_addr:
toc_pointer:
		.long	s.got
bss_length:
		.long	bss.size
bss_addr:
		.long	bss.start
sbss_length:
		.long	sbss.size
sbss_addr:
		.long	sbss.start
stack_top:
		.long	stack.end
PUBLIC_VAR (text_addr)
text_addr:
        .long   text.start
PUBLIC_VAR (text_length)
text_length:
        .long   text.size

/*---------------------------------------------------------------------------
 * Reset_entry.
 *--------------------------------------------------------------------------*/
.startup:
			/* Get entrypoint address in R1 so we can find linker variables */
			mflr	r1

			/* Initialise procesor registers generally */
			bl		init405

			/* Clear .bss and .sbss */
			bl		bssclr

			/*-------------------------------------------------------------------
			 * C_setup.
			 *------------------------------------------------------------------*/
			lwz     r1,stack_top - base_addr(r1)         /* Now set R1 to stack_top */
			addi    r1,r1,-56-4         /* start stack at text_addr - 56 */
			li		r3,0
			stw		r3, 0(r1)			/* Clear stack chain */
			stw		r3, 4(r1)
			stw		r3, 8(r1)
			stw		r3, 12(r1)

			bl		__eabi				/* Initialise EABI: sets up r2 & r13 */

			li		r3, 0			/* command line */

			b       SYM (boot_card) 	 /* call the first C routine */

/*---------------------------------------------------------------------------
 * bssclr.
 *--------------------------------------------------------------------------*/
bssclr:		lwz     r2,bss_addr-base_addr(r1)   /* start of bss set by loader */
			lwz     r3,bss_length-base_addr(r1) /* bss length */
			srwi.	r3,r3,2		/* div 4 to get # of words */
			li		r0,0
			beq		dosbss					/* no bss */
			mtctr   r3                      /* set ctr reg */
			subi	r2,r2,4
clear_bss:	stwu	r0,4(r2)
			bdnz    clear_bss               /* decrement counter and loop */

dosbss:		lwz     r2,sbss_addr-base_addr(r1)   /* start of sbss set by loader */
			lwz     r3,sbss_length-base_addr(r1) /* sbss length */
			slwi.	r3,r3,2					/* div 4 to get # of words */
			subi	r2,r2,4
			beqlr                           /* no sbss */
			mtctr   r3                      /* set ctr reg */
clear_sbss:	stwu	r0,4(r2)
			bdnz    clear_sbss               /* decrement counter and loop */

			blr                             /* return */


/*---------------------------------------------------------------------------
 * Generic 405 register setup
 *--------------------------------------------------------------------------*/
init405:
			li		r0, 0
			mtmsr	r0
			mticcr	r0
			mtdccr	r0

			li		r3,0x7FFC	# 405EX-specific
			mtsgr	r3			# Clear guarded mode on all storage except PCIe region

			mtsler	r0			# Storage is all big-endian
			mtsu0r	r0			# and uncompressed

			iccci	r3,0			# Invalidate the instruction cache
			li		r3,1			# Enable F800 0000 to FFFF FFFF
			oris	r3,r3,0xC000	# Enable 0000 0000 to 0FFF FFFF
			mticcr	r3
			isync

			li		r3,0
			li		r4,256		# 405 has 128 or 256 32-byte lines: do 256
			mtctr	r4			# set loop ctr
dcloop:		dccci	0,r3		# invalidate line
			addi	r3,r3,0x20	# bump to next line
			bdnz	dcloop
			mtdcwr	r0			# Select write-back caching
			lis		r3,0xC000	# Enable 0000 0000 to 0FFF FFFF
			# mtdccr	r3			# Enable data cache

			mtevpr	r0
			mtesr	r0
			mtxer	r0

			lwarx   r3,r0,r0		# get some data/set resv bit
			stwcx.	r3,r0,r0		# store out and clear resv bit

			lis		r3,0xDEAD
			ori		r3,r3,0xBEEF	# Make distintive uninitialised value
			mr		r4, r3
			mr		r5, r3
			mr		r6, r3
			mr		r7, r3
			mr		r8, r3
			mr		r9, r3
			mr		r10, r3
			mr		r11, r3
			mr		r12, r3
			mr		r13, r3
			mr		r14, r3
			mr		r15, r3
			mr		r16, r3
			mr		r17, r3
			mr		r18, r3
			mr		r19, r3
			mr		r20, r3
			mr		r21, r3
			mr		r22, r3
			mr		r23, r3
			mr		r24, r3
			mr		r25, r3
			mr		r26, r3
			mr		r27, r3
			mr		r28, r3
			mr		r29, r3
			mr		r30, r3
			mr		r31, r3

			blr

.L_text_e:

			.comm	environ,4,4
