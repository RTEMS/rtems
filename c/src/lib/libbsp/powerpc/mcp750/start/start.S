/*
 *  arch/ppc/kernel/head.S
 *
 *  $Id$
 *
 *  PowerPC version 
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@linuxppc.org)
 *
 *  Rewritten by Cort Dougan (cort@cs.nmt.edu) for PReP
 *  Adapted for Power Macintosh by Paul Mackerras.
 *  Low-level exception handlers and MMU support
 *  rewritten by Paul Mackerras.
 *    Copyright (C) 1996 Paul Mackerras.
 *  MPC8xx modifications Copyright (C) 1997 Dan Malek (dmalek@jlc.net).
 *  Amiga/APUS changes by Jesper Skov (jskov@cygnus.co.uk).
 *
 *  This file contains the low-level support and setup for the
 *  PowerPC platform, including trap and interrupt dispatch.
 *  Also included here is low-level thread/task switch support.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *	
 */

#include <libcpu/cpu.h>
#include <libcpu/io.h>
#include <rtems/score/targopts.h>
#include "asm.h"
	
#define SYNC \
	sync; \
	isync

#define KERNELBASE	0x0

#define MONITOR_ENTER			\
	mfmsr	r10		;	\
	ori	r10,r10,MSR_IP	;	\
	mtmsr	r10		;	\
	li	r10,0x63	;	\
	sc
			
	.text
	.globl	__rtems_entry_point
	.type	__rtems_entry_point,@function
__rtems_entry_point:
#ifdef DEBUG_EARLY_START
	MONITOR_ENTER
#endif	
		
/* 
 * PREP
 * This is jumped to on prep systems right after the kernel is relocated
 * to its proper place in memory by the boot loader.  The expected layout
 * of the regs is:	
 *   r3: ptr to residual data
 *   r4: initrd_start or if no initrd then 0
 *   r5: initrd_end - unused if r4 is 0
 *   r6: Start of command line string
 *   r7: End of command line string
 *
 */
	
	mr	r31,r3			/* save parameters */
	mr	r30,r4
	mr	r29,r5
	mr	r28,r6
	mr	r27,r7
/*
 * Use the first pair of BAT registers to map the 1st 64MB
 * of RAM to KERNELBASE. 
 */
	lis	r11,KERNELBASE@h
	ori	r11,r11,0x7fe		/* set up BAT registers for 604 */
	li	r8,2			/* R/W access */
	mtspr	DBAT0L,r8		/* N.B. 6xx (not 601) have valid */
	mtspr	DBAT0U,r11		/* bit in upper BAT register */
	mtspr	IBAT0L,r8
	mtspr	IBAT0U,r11
	isync

/*
 * we now have the 1st 64M of ram mapped with the bats.
 */
	
enter_C_code:
	bl	MMUon
	/*
	 * stack = &__rtems_end + 4096
	 */
	addis	r9,r0, __rtems_end+4096@ha
        addi	r9,r9, __rtems_end+4096@l
	mr	r1, r9
	bl	zero_bss
	/*
	 * restore prep boot params
	 */
	mr	r3,r31
	mr	r4,r30
	mr	r5,r29
	mr	r6,r28
	mr	r7,r27 
	bl	save_boot_params
	bl	boot_card
	bl	_return_to_ppcbug
	
	.globl  MMUon
	.type	MMUon,@function
MMUon:	
	mfmsr	r0
	ori	r0,r0, MSR_IP | MSR_RI | MSR_IR | MSR_DR | MSR_EE | MSR_FE0 | MSR_FE1
	xori	r0, r0, MSR_EE | MSR_IP | MSR_FP
	mflr	r11
	mtsrr0	r11
	mtsrr1	r0
	SYNC
	rfi
	
	.globl  MMUoff
	.type	MMUoff,@function
MMUoff:	
	mfmsr	r0
	ori	r0,r0,MSR_IR| MSR_DR | MSR_IP
	mflr	r11
	xori	r0,r0,MSR_IR|MSR_DR
	mtsrr0	r11
	mtsrr1	r0
	SYNC
	rfi

	.globl	_return_to_ppcbug
	.type	_return_to_ppcbug,@function

	
_return_to_ppcbug:
	mflr	r30
	bl	MMUoff
	MONITOR_ENTER
	bl	MMUon
	mtctr	r30
	bctr	
	
	



