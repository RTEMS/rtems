/*  start.S
 *
 *  Modified for the Motorola PQII ADS board by
 *  Andy Dachs <a.dachs@sstl.co.uk> 23-11-00.
 *  Surrey Satellite Technology Limited
 *
 *  I have a proprietary bootloader programmed into the flash
 *  on the board which initialises the SDRAM prior to calling
 *  this function.
 *
 *  This file is based on the one by Jay Monkman (jmonkman@fracsa.com)
 *  which in turn was based on the dlentry.s file for the Papyrus BSP,
 *  written by:
 *
 *  Author:     Andrew Bray <andy@i-cubed.co.uk>
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
 */

#include <rtems/asm.h>

/*
 *  The initial stack is set to run BELOW the code base address.
 *  (between the vectors and text sections)
 *
 *  The entry veneer has to clear the BSS and copy the read only
 *  version of the data segment to the correct location.
 */

        .section ".entry"  /* This might have to be the first thing in the
                            * text section. At one time, it had to be
                            * first, but I don't believe it is true
                            * any more. */
        PUBLIC_VAR (start)
SYM(start):
        bl      .startup
base_addr:

/*
 * Parameters from linker
 */
toc_pointer:
        .long   s.got
bss_length:
        .long   bss.size
bss_addr:
        .long   bss.start

PUBLIC_VAR (data_length )
data_length:
	.long	data.size
PUBLIC_VAR (data_addr )
data_addr:
	.long	data.start

PUBLIC_VAR (text_addr)
text_addr:
        .long   text.start

PUBLIC_VAR (text_length)
text_length:
        .long   text.size

/*
 * Initialization code
 */
.startup:
    /* Get start address */
        mflr    r1

    /* --------------------------------------------------
     * Clear MSR[EE] to disable interrupts
     * Clear MSR[IP] bit to put vectors at 0x00000000
     * Set MSR[FP] to enable FPU - not on my eval board!
     * -------------------------------------------------- */
	mfmsr	r5
	lis	r13, 0xFFFF
	ori	r13, r13, 0x7FBF
	and 	r5, r5, r13			/* Clear EE and IP */
#if 1
	ori	r5, r5, 0x2000    		/* Enable FPU */
#endif
	mtmsr	r5

#ifdef ENABLE_CACHE
	/* Enable caches */
	mfspr	r5, 1008
	ori	r5, r5, 0x8000
	isync
	mtspr	1008, r5

/*	Leave D-cache disabled for now */
#if 0
	ori	r5, r5, 0x4000
	sync
	mtspr	1008, r5
#endif
#endif

	/*--------------------------------------------------
	 * Set up the power management modes
	 * The 8260 has a dynamic power management mode that
	 * is automatically invoked if the unit is idle.
	 * We invoke the NAP mode in the RTEMS idle task.
	 *-------------------------------------------------- */

	lis	r13, 0x0050     /* set nap mode and DPM */
	or	r5, r5, r13
	mtspr	1008, r5

	/*--------------------------------------------------
	 *
	 *-------------------------------------------------- */

        /* clear the bss section */
        bl      bssclr

/*
 * C_setup.
 */

        /* set toc */
        lwz r2, toc_pointer-base_addr(r1)

        /* Set up stack pointer = beginning of text section - 56 */
        addi    r1, r1, -56-4

        /* Clear cmdline */
        xor r3, r3, r3

        .extern SYM (boot_card)
        bl       SYM (boot_card)	/* call the first C routine */

	/* we don't expect to return from boot_card but if we do */
	/* wait here for watchdog to kick us into hard reset     */

twiddle:
	b 		twiddle

/*
 * bssclr - zero out bss
 */
bssclr:
        lwz     r4, bss_addr-base_addr(r1)      /* Start of bss */
        lwz     r5, bss_length-base_addr(r1)    /* Length of bss */

        rlwinm. r5,r5,30,0x3FFFFFFF             /* form length/4 */
        beqlr                                   /* no bss */
        mtctr   r5                              /* set ctr reg */
        xor     r6,r6,r6                        /* r6 = 0 */
clear_bss:
        stswi   r6,r4,0x4                       /* store r6 */
        addi    r4,r4,0x4                       /* update r2 */

        bdnz    clear_bss                       /* dec counter and loop */
        blr                                     /* return */
