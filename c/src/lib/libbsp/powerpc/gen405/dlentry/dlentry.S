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
 *  $Id$
 *
 *  derived from "helas403/dlentry.S":	
 *  Id: dlentry.S,v 1.2 2000/08/02 16:30:57 joel Exp 
 */

#include "asm.h"

/*
 *  The gen405 ELF link scripts support three special sections:
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

#if PPC_ASM == PPC_ASM_ELF
        .section .entry
#else
	.csect .text[PR]
#endif

	PUBLIC_VAR (download_entry)
SYM(download_entry):
	bl	.startup
base_addr:	

/*---------------------------------------------------------------------------
 * Parameters from linker
 *--------------------------------------------------------------------------*/
toc_pointer:	
#if PPC_ASM == PPC_ASM_ELF
	.long	s.got
#else
	.long	TOC[tc0]
#endif
bss_length:	
	.long	bss.size
bss_addr:	
	.long	bss.start
stack_top:
	.long	stack.end
/*---------------------------------------------------------------------------
 * Reset_entry.
 *--------------------------------------------------------------------------*/
.startup:
	/* Get start address, stack grows down from here... */
	mflr	r1

	/* Assume Bank regs set up..., cache etc. */
        bl      bssclr

	.extern SYM(__vectors)

	lis	r2,__vectors@h          /* set EVPR exc. vector prefix */
	mtspr	evpr,r2	

        /*-------------------------------------------------------------------
         * C_setup.
         *------------------------------------------------------------------*/
        lwz     r2,toc_pointer-base_addr(r1)       /* set r2 to toc */
        lwz     r1,stack_top-base_addr(r1)         /* set r1 to stack_top */

        addi    r1,r1,-56-4              /* start stack at text_addr - 56 */
        addi    r3,r0,0x0                /* clear r3 */
	stw	r3, 0(r1)		 /* Clear stack chain */
	stw	r3, 4(r1)
	stw	r3, 8(r1)
	stw	r3, 12(r1)
        lis	r5,environ@ha
        la	r5,environ@l(r5)	/* environp */
	li	r4, 0			/* argv */
	li	r3, 0			/* argc */
        .extern SYM (boot_card)
        b       SYM (boot_card) 	 /* call the first C routine */

/*---------------------------------------------------------------------------
 * bssclr.
 *--------------------------------------------------------------------------*/
bssclr:
        /*-------------------------------------------------------------------
         * Data move finished, zero out bss.
         *------------------------------------------------------------------*/
        lwz     r2,bss_addr-base_addr(r1)   /* start of bss set by loader */
        lwz     r3,bss_length-base_addr(r1) /* bss length */
        rlwinm. r3,r3,30,0x3FFFFFFF     /* form length/4 */
        beqlr                           /* no bss */
        mtctr   r3                      /* set ctr reg */
        xor     r6,r6,r6                /* r6 = 0 */
clear_bss:
        stswi   r6,r2,0x4               /* store r6 */
        addi    r2,r2,0x4               /* update r2 */
        bdnz    clear_bss               /* decrement counter and loop */
        blr                             /* return */
.L_text_e:

#if PPC_ABI == PPC_ABI_POWEROPEN
	DESCRIPTOR (startup)
#endif

	.comm	environ,4,4

