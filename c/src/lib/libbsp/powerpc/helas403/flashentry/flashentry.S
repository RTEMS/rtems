/*  flashentry.s
 *
 *  This file contains the entry code for RTEMS programs starting
 *  directly from Flash. 
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
 *  stored in Papyrus' flash ROM.
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
 */

#include "asm.h"
#include <rtems/score/ppc.h>	
				
/*----------------------------------------------------------------------------
 * Reset_entry.
 *---------------------------------------------------------------------------*/
#if PPC_ASM == PPC_ASM_ELF
        .section .reset,"ax",@progbits 
	/* this section MUST be located at absolute address 0xFFFFFFFC
	   or last word of EPROM */
#else
	.csect .text[PR]
#endif

	ba	flash_entry /* this is the first instruction after reset */

	.previous
	
/*----------------------------------------------------------------------------
 * ROM Vector area.
 *---------------------------------------------------------------------------*/
#if PPC_ASM == PPC_ASM_ELF
        .section .entry,"ax",@progbits
#else
	.csect .text[PR]
#endif
	PUBLIC_VAR (flash_entry)
SYM (flash_entry):
	bl	.startup /* call startup, link reg points to base_addr */
base_addr:
/*----------------------------------------------------------------------------
 * Parameters from linker
 *---------------------------------------------------------------------------*/
toc_pointer:
#if PPC_ASM == PPC_ASM_ELF
	.long	s.got
#else
	.long	TOC[tc0]
#endif
text_length:	
	.long	text.size
text_addr:
	.long	text.start
copy_src:	
	.long	copy.src
copy_length:
	.long	copy.size
copy_dest:	
	.long	copy.dest
bss_length:	
	.long	bss.size
bss_addr:	
	.long	bss.start
stack_top:
	.long	stack.end
	
/*----------------------------------------------------------------------------
 * from Reset_entry.
 *---------------------------------------------------------------------------*/
.startup:
	/* Get start address, r1 points to label base_addr */
	mflr	r1

	/* Set up Bank regs, cache etc. */
	/* set up bank register BR0 for Flash-EPROM:
         * NOTE: bank size should stay 1MByte, this is standard size
         *       after RESET
	 * base addr = Fffxxxxx -> 0b11111111........................ 
	 * bank size = 1 MByte  -> 0b........000..................... (std)
	 * bank use  = readonly -> 0b...........01...................
	 * seq. fill = targ frst-> 0b.............0.................. 
	 * burst mode= enable   -> 0b..............1.................
	 * bus width = 8 bit    -> 0b...............00............... 
	 * ready pin = disable  -> 0b.................0..............
	 * first wait= 2 clocks -> 0b..................0010..........
	 * burst wait= 2 clocks -> 0b......................10........
	 * CSon time = 0 clocks -> 0b........................0.......
	 * OEon time = 0 clocks -> 0b.........................0......
	 * WBon time = 1 clocks -> 0b..........................1.....
	 * WBoff time= 0 clocks -> 0b...........................0....
	 * Hold time = 1 clocks -> 0b............................001.
	 * ram type  = SRAM(ign)-> 0b...............................1
	 * value                   0b11111111000010100000101000100011
	 *                         0x   F   F   0   A   0   A   2   3
	 */
	lis	r2,0xFF0A
	ori	r2,r2,0x0A23

	mtdcr	br0,r2 /* write to DCR BR0 */


	/*--------------------------------------------------------------------
	 * test various RAM configurations (from big to small per bank)
	 *------------------------------------------------------------------*/
	/*--------------------------------------------------------------------
	 * test RAM config 16 MByte (1x4Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 for DRAM:
	 * base addr = 000xxxxx -> 0b00000000........................ 
	 * bank size = 16MByte  -> 0b........100.....................
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00000000100110010010101010110000
	 *                         0x   0   0   9   9   2   A   B   0
	 */
	lis	r2,0x0099
	ori	r2,r2,0x2AB0
	mtdcr	br7,r2 /* write to DCR BR7*/	

	lis	r2,0x0000       /* start address = 0x00000000 */
	lis	r3,0x0100       /* size  16 MB   = 0x01000000 */
	bl	ramacc		/* test memory accessibility  */
	cmpi	0,0,r4,0	/* memory ok? else test smaller size */
	bne	ramcfgt18

	/*--------------------------------------------------------------------
	 * test RAM config 32 MByte (2x4Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 like above
	 * set up bank register BR6 for DRAM:	
	 * base addr = 010xxxxx -> 0b00010000........................ 
	 * bank size = 16MByte  -> 0b........100..................... (for now)
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00010000100110010010101010110000
	 *                         0x   1   0   9   9   2   A   B   0
	 */
	lis	r2,0x1099
	ori	r2,r2,0x2AB0
	mtdcr	br6,r2		/* write to DCR BR6*/	

	lis	r2,0x0100       /* start address = 0x01000000 */
	lis	r3,0x0100       /* size  16 MB   = 0x01000000 */
	bl	ramacc		/* test memory accessibility  */
	cmpi	0,0,r4,0	/* memory ok? else test smaller size */
	beq	ramcfgok	/* ok, we found configuration... +/
	
	lis	r2,0x0000	/* disable BR6, config not ok */
	mtdcr	br6,r2		/* write to DCR BR6*/	
	b	ramcfgok	/* and finish configuration */
	
ramcfgt18:			
	/*--------------------------------------------------------------------
	 * test RAM config 8 MByte (1x2Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 for DRAM:
	 * base addr = 000xxxxx -> 0b00000000........................ 
	 * bank size =  8MByte  -> 0b........011.....................
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00000000011110010010101010110000
	 *                         0x   0   0   7   9   2   A   B   0
	 */
	lis	r2,0x0079
	ori	r2,r2,0x2AB0
	mtdcr	br7,r2         /* write to DCR BR7           */	

	lis	r2,0x0000       /* start address = 0x00000000 */
	lis	r3,0x0080       /* size   8 MB   = 0x00800000 */
	bl	ramacc		/* test memory accessibility  */
	cmpi	0,0,r4,0	/* memory ok? else test smaller size */
	bne	ramcfgt14

	/*--------------------------------------------------------------------
	 * test RAM config 16 MByte (2x2Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 like above
	 * set up bank register BR6 for DRAM:	
	 * base addr = 008xxxxx -> 0b00001000........................ 
	 * bank size = 08MByte  -> 0b........011..................... (for now)
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00001000011110010010101010110000
	 *                         0x   0   8   7   9   2   A   B   0
	 */
	lis	r2,0x0879
	ori	r2,r2,0x2AB0
	mtdcr	br6,r2		/* write to DCR BR6*/	

	lis	r2,0x0080       /* start address = 0x00800000 */
	lis	r3,0x0080       /* size   8 MB   = 0x00800000 */
	bl	ramacc		/* test memory accessibility  */
	cmpi	0,0,r4,0	/* memory ok? else test smaller size */
	beq	ramcfgok	/* ok, we found configuration... +/
	
	lis	r2,0x0000	/* disable BR6, config not ok */
	mtdcr	br6,r2		/* write to DCR BR6*/	
	b	ramcfgok	/* and finish configuration */
	
ramcfgt14:			
	/*--------------------------------------------------------------------
	 * test RAM config 4 MByte (1x1Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 for DRAM:
	 * base addr = 000xxxxx -> 0b00000000........................ 
	 * bank size =  4MByte  -> 0b........010.....................
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00000000010110010010101010110000
	 *                         0x   0   0   5   9   2   A   B   0
	 */
	/*
         * FIXME: this is the minimum size supported, should test and
         * report error, when failed
         */
	lis	r2,0x0059
	ori	r2,r2,0x2AB0
	mtdcr	br7,r2 /* write to DCR BR7*/	

	/*--------------------------------------------------------------------
	 * test RAM config  8 MByte (2x1Mx32Bit)
	 *------------------------------------------------------------------*/
	/* set up bank register BR7 like above
	 * set up bank register BR6 for DRAM:	
	 * base addr = 004xxxxx -> 0b00000100........................ 
	 * bank size =  4MByte  -> 0b........010..................... (for now)
	 * bank use  = readwrite-> 0b...........11...................
	 * seq. fill = targ.frst-> 0b.............0.................. 
	 * early RAS = disabled -> 0b..............0.................
	 * bus width = 32bit    -> 0b...............10............... 
	 * adr mux   = internal -> 0b.................0..............
	 * RAS to CAS= 2 clocks -> 0b..................1.............
	 * Alt. Rfrsh= normal   -> 0b...................0............
	 * page mode = enabled  -> 0b....................1...........
	 * first wait= 1 clocks -> 0b.....................01.........
	 * burst wait= 1 clocks -> 0b.......................01.......
	 * precharge = 1 clocks -> 0b.........................0......
	 * RAS Rfrsh = 2 clocks -> 0b..........................1.....
	 * Rfrsh Itvl= 512 clks -> 0b...........................1000.
	 * ram type  = DRAM     -> 0b...............................0
	 * value                   0b00000100010110010010101010110000
	 *                         0x   0   4   5   9   2   A   B   0
	 */
	lis	r2,0x0459
	ori	r2,r2,0x2AB0
	mtdcr	br6,r2		/* write to DCR BR6*/	

	lis	r2,0x0040       /* start address = 0x00400000 */
	lis	r3,0x0040       /* size   4 MB   = 0x00400000 */
	bl	ramacc		/* test memory accessibility  */
	cmpi	0,0,r4,0	/* memory ok? else test smaller size */
	beq	ramcfgok	/* ok, we found configuration... +/
	
	lis	r2,0x0000	/* disable BR6, config not ok */
	mtdcr	br6,r2		/* write to DCR BR6*/	
	b	ramcfgok	/* and finish configuration */

ramcfgok:	
        /*--------------------------------------------------------------------
         * init the DRAM where STACK+ DATA+ BBS will be placed.  If this is OK
         * we will return here.
         *-------------------------------------------------------------------*/
        bl      rom2ram

	/* clear caches */
	addi	r2,0,PPC_I_CACHE/PPC_CACHE_ALIGNMENT
	mtctr	r2		/* count the loops needed... */
	xor	r2,r2,r2	/* start at adr zero */
icinvlp:	
	iccci	0,r2
	addi	r2,r2,PPC_CACHE_ALIGNMENT
	bdnz	icinvlp
	
	addi	r2,r0,PPC_D_CACHE/PPC_CACHE_ALIGNMENT
	mtctr	r2		/* count the loops needed... */
	xor	r2,r2,r2	/* start at adr 0 */
dcinvlp:		
	dccci	0,r2
	addi	r2,r2,PPC_CACHE_ALIGNMENT
	bdnz	dcinvlp
        /*--------------------------------------------------------------------
         * Enable two 128MB cachable regions.
	 * FEPROM is cachable    at 0xFFF00000..0xFFFFFFFF
	 * DRAM   is cachable    at 0x00000000..0x00FFFFFF	
	 * FEPROM is noncachable at 0x7FF00000..0x7FFFFFFF
	 * DRAM   is noncachable at 0x80000000..0x80FFFFFF	
         *-------------------------------------------------------------------*/
        addis   r2,r0,0x8000
        addi    r2,r2,0x0001

        mtspr   iccr, r2               /* ICCR */
        mtspr   dccr, r2               /* DCCR */

	.extern SYM(__vectors)

	lis	r2,__vectors@h          /* set EVPR exc. vector prefix */
	mtspr	evpr,r2

	lis     r2,0x0000
        ori     r2,r2,0x0000
	mtmsr   r2                      /* set default msr           */
        lis     r2,0x0000               /* do not allow critical IRQ */
        ori     r2,r2,0x0000
        mtdcr   exier, r2               /* disable all external IRQs */
	
        addi    r2,r0,-1                /* r2 = 0xffffffff               */
        mtdcr   exisr, r2               /* clear all pendingdisable IRQs */
	
        /*--------------------------------------------------------------------
         * C_setup.
         *-------------------------------------------------------------------*/

        lwz     r2,toc_pointer-base_addr(r1)       /* set r2 to toc */
        lwz     r1,stack_top-base_addr(r1)         /* set r1 to stack_top */
	
        addi    r1,r1,-56                /* start stack at data_addr - 56 */
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

/*----------------------------------------------------------------------------
 * Rom2ram.
 *---------------------------------------------------------------------------*/
rom2ram:
        lwz     r2,copy_dest-base_addr(r1)    /* start of data set by loader */
        lwz     r3,copy_length-base_addr(r1)  /* data length                 */
        rlwinm  r3,r3,30,0x3FFFFFFF           /* form length/4               */
        mtctr   r3                            /* set ctr reg                 */
        /*--------------------------------------------------------------------
         * Calculate offset of data in image.
         *-------------------------------------------------------------------*/
        lwz     r4,copy_src-base_addr(r1)      /* get start of copy area */
move_data:
        lswi    r6,r4,0x4               /* load r6 */
        stswi   r6,r2,0x4               /* store r6 */
        addi    r4,r4,0x4               /* update r4 */
        addi    r2,r2,0x4               /* update r2 */
        bdnz    move_data               /* decrement counter and loop */
        /*--------------------------------------------------------------------
         * Data move finished, zero out bss.
         *-------------------------------------------------------------------*/
        lwz     r2,bss_addr-base_addr(r1) /* start of bss set by loader */
        lwz     r3,bss_length-base_addr(r1)       /* bss length */
        rlwinm. r3,r3,30,0x3FFFFFFF     /* form length/4 */
        beqlr                           /* no bss */
        mtctr   r3                      /* set ctr reg */
        xor     r6,r6,r6                /* r6 = 0 */
clear_bss:
        stswi   r6,r2,0x4               /* store r6 */
        addi    r2,r2,0x4               /* update r2 */
        bdnz    clear_bss               /* decrement counter and loop */
        blr                             /* return */

/*----------------------------------------------------------------------------
 * ramacc test accessibility of RAM
 * input:	r2 = start address, r3 = length (in byte)
 * output:	r4 = 0 -> ok, !=0 -> fail
 *---------------------------------------------------------------------------*/
ramacc:
	xor	r4,r4,r4		/* r4 = 0                    */
	stw	r4,0(r2)		/* init ram at start address */
	addi	r4,r0,0x04              /* set start shift           */
ramaccf1:	
	cmp     0,0,r4,r3               /* compare with length       */
	bge     ramaccfx		/* r4 >= r3? then finished   */
	add	r5,r4,r2		/* get next address to fill  */
	stw	r4,0(r5)		/* store new pattern         */
	add	r4,r4,r4		/* r4 = r4*2                 */
	b	ramaccf1		/* and then next loop        */

ramaccfx:
	lwz	r4,0(r2)		/* get memory at start adr  */
	blr
	

#if PPC_ABI == PPC_ABI_POWEROPEN
	DESCRIPTOR (startup)
#endif


	.comm	environ,4,4


