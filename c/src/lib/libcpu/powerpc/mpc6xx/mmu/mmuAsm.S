/*
 *  mmuAsm.S
 *
 *  $Id$
 *
 *  Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *
 *  This file contains the low-level support for various MMU
 *  features.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *	
 *  T. Straumann - 11/2001: added support for 7400 (no AltiVec yet)
 */

#include <asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/io.h>

/* Unfortunately, the CPU types defined in cpu.h are
 * an 'enum' type and hence not available :-(
 */
#define PPC_601   0x1
#define PPC_603   0x3
#define PPC_604   0x4
#define PPC_603e  0x6
#define PPC_603ev 0x7
#define PPC_750   0x8
#define PPC_604e  0x9
#define PPC_604r  0xA
#define PPC_7400  0xC
#define PPC_620   0x16
#define PPC_860   0x50
#define PPC_821   PPC_860
#define PPC_8260  0x81

/* ALTIVEC instructions (not recognized by off-the shelf gcc yet) */
#define DSSALL	.long	0x7e00066c		/* DSSALL altivec instruction opcode */

/* A couple of defines to make the code more readable */
#define CACHE_LINE_SIZE 32

#ifndef MSSCR0
#define MSSCR0   1014
#else
#warning MSSCR0 seems to be known, update __FILE__
#endif

#define DL1HWF	(1<<(31-8))
#define L2HWF	(1<<(31-20))



/*
 * Each setdbat routine start by invalidating the DBAT as some
 * proc (604e) request the valid bit set to 0 before accepting 
 * to write in BAT
 */

	.globl  asm_setdbat0
	.type	asm_setdbat0,@function
asm_setdbat0:
	li	r0,0
	sync
	isync
	mtspr	DBAT0U,r0
	mtspr	DBAT0L,r0
	sync
	isync
	mtspr DBAT0L, r4
	mtspr DBAT0U, r3
	sync
	isync
	blr

	.globl  asm_setdbat1
	.type	asm_setdbat1,@function
asm_setdbat1:
	li	r0,0
	sync
	isync
	mtspr	DBAT1U,r0
	mtspr	DBAT1L,r0
	sync
	isync
	mtspr DBAT1L, r4
	mtspr DBAT1U, r3
	sync
	isync
	blr

	.globl  asm_setdbat2
	.type	asm_setdbat2,@function
asm_setdbat2:	
	li	r0,0
	sync
	isync
	mtspr	DBAT2U,r0
	mtspr	DBAT2L,r0
	sync
	isync
	mtspr DBAT2L, r4
	mtspr DBAT2U, r3
	sync
	isync
	blr

	.globl  asm_setdbat3
	.type	asm_setdbat3,@function
asm_setdbat3:	
	li	r0,0
	sync
	isync
	mtspr	DBAT3U,r0
	mtspr	DBAT3L,r0
	sync
	isync
	mtspr DBAT3L, r4
	mtspr DBAT3U, r3
	sync
	isync
	blr
		
	.globl L1_caches_enables
	.type  L1_caches_enables, @function
	
L1_caches_enables:	
	/*
	 * Enable caches and 604-specific features if necessary.
	 */
	mfspr	r9,PVR
	rlwinm	r9,r9,16,16,31
	cmpi	0,r9,PPC_601
	beq	4f			/* not needed for 601 */
	mfspr	r11,HID0
	andi.	r0,r11,HID0_DCE
	ori	r11,r11,HID0_ICE|HID0_DCE
	ori	r8,r11,HID0_ICFI
	bne	3f			/* don't invalidate the D-cache */
	ori	r8,r8,HID0_DCI		/* unless it wasn't enabled */
3:
	sync
	mtspr	HID0,r8			/* enable and invalidate caches */
	sync
	mtspr	HID0,r11		/* enable caches */
	sync
	isync
	cmpi	0,r9,PPC_604	/* check for 604 */
	cmpi	1,r9,PPC_604e	/* or 604e */
	cmpi	2,r9,PPC_604r	/* or mach5 */
	cror	2,2,6
	cror	2,2,10
	cmpi	1,r9,PPC_750	/* or 750 */
	cror	2,2,6
	cmpi	1,r9,PPC_7400	/* or 7400 */
	bne	3f
	ori	r11,r11,HID0_BTIC	/* enable branch tgt cache on 7400 */
3:	cror	2,2,6
	bne	4f
	/* on 7400 SIED is actually SGE (store gathering enable) */
	ori	r11,r11,HID0_SIED|HID0_BHTE /* for 604[e], enable */
	bne	2,5f
	ori	r11,r11,HID0_BTCD
5:	mtspr	HID0,r11		/* superscalar exec & br history tbl */
4:
	blr
	
	.globl get_L2CR
	.type  get_L2CR, @function	
get_L2CR:	
	/* Make sure this is a > 750 chip */
	mfspr	r3,PVR
	rlwinm	r3,r3,16,16,31
	cmplwi	r3,PPC_750	/* it's a 750 */
	beq	1f
	cmplwi  r3,PPC_7400	/* it's a 7400 */
	beq	1f
	li	r3,0
	blr
	
1:
	/* Return the L2CR contents */
	mfspr	r3,L2CR
	blr

	.globl set_L2CR
	.type  set_L2CR, @function
set_L2CR:	
	/* Usage:
	 * When setting the L2CR register, you must do a few special things.  
	 * If you are enabling the cache, you must perform a global invalidate. 
	 * If you are disabling the cache, you must flush the cache contents first.
	 * This routine takes care of doing these things.  When first
	 * enabling the cache, make sure you pass in the L2CR you want, as well as 
	 * passing in the global invalidate bit set.  A global invalidate will 
	 * only be performed if the L2I bit is set in applyThis.  When enabling 
	 * the cache, you should also set the L2E bit in applyThis.  If you
	 * want to modify the L2CR contents after the cache has been enabled, 
	 * the recommended procedure is to first call __setL2CR(0) to disable 
	 * the cache and then call it again with the new values for L2CR.  Examples:
	 *
	 *	_setL2CR(0)		-	disables the cache
	 *	_setL2CR(0xb9A14000)	-	enables my G3 MCP750 card:
	 *				-	L2E set to turn on the cache
	 *				-	L2SIZ set to 1MB
	 *				-	L2CLK set to %2
	 *				-	L2RAM set to pipelined syncronous late-write
	 *				-	L2I set to perform a global invalidation
	 *				-	L2OH set to 1 nS
	 *
	 * A similar call should work for your card.  You need to know the correct 
	 * setting for your card and then place them in the fields I have outlined 
	 * above.  Other fields support optional features, such as L2DO which caches 
	 * only data, or L2TS which causes cache pushes from the L1 cache to go to 
	 *the L2 cache instead of to main memory.
	 */
	
	/* Make sure this is a > 750 chip */
	mfspr	r0,PVR
	rlwinm	r0,r0,16,16,31
	cmplwi	r0,PPC_750
	beq	thisIs750
	cmplwi	r0,PPC_7400
	beq	thisIs750
	li	r3,-1
	blr
	
thisIs750:
	/* Get the current enable bit of the L2CR into r4 */
	mfspr	r4,L2CR
	rlwinm	r4,r4,0,0,0
	
	/* See if we want to perform a global inval this time. */
	rlwinm	r6,r3,0,10,10		/* r6 contains the new invalidate bit */
	rlwinm.	r5,r3,0,0,0			/* r5 contains the new enable bit */
	rlwinm	r3,r3,0,11,9		/* Turn off the invalidate bit */
	rlwinm	r3,r3,0,1,31		/* Turn off the enable bit */
	or		r3,r3,r4			/* Keep the enable bit the same as it was for now. */
	mfmsr	r7					/* shut off interrupts around critical flush/invalidate sections */
	rlwinm	r4,r7,0,17,15		/* Turn off EE bit - an external exception while we are flushing
								   the cache is fatal (comment this line and see!) */
	mtmsr	r4
	bne		dontDisableCache	/* Only disable the cache if L2CRApply has the enable bit off */

	cmplwi  r0,PPC_7400			/* > 7400 ? */ 
	bne		disableCache		/* use traditional method */

	/* On the 7400, they recommend using the hardware flush feature */
	DSSALL						/* stop all data streams */
	sync
	/* we wouldn't have to flush L1, but for sake of consistency with the other code we do it anyway */
	mfspr	r4, MSSCR0
	oris	r4, r4, DL1HWF@h
	mtspr	MSSCR0, r4
	sync
	/* L1 flushed */
	mfspr	r4, L2CR
	ori		r4, r4, L2HWF
	mtspr	L2CR, r4
	sync
	/* L2 flushed */
	b		flushDone

disableCache:
	/* Disable the cache.  First, we turn off data relocation. */
	rlwinm	r4,r4,0,28,26		/* Turn off DR bit */
	mtmsr	r4
	isync						/* make sure memory accesses have completed */
	
	/*
		Now, read the first 2MB of memory to put new data in the cache.
		(Actually we only need the size of the L2 cache plus
		the size of the L1 cache, but 2MB will cover everything just to be safe).
	*/
	lis		r4,0x0001
	mtctr	r4
	li		r4,0
loadLoop:
	lwzx	r0,r0,r4
	addi	r4,r4,CACHE_LINE_SIZE	/* Go to start of next cache line */
	bdnz	loadLoop
	
	/* Now, flush the first 2MB of memory */
	lis		r4,0x0001
	mtctr	r4
	li		r4,0
	sync
flushLoop:
	dcbf	r0,r4
	addi	r4,r4,CACHE_LINE_SIZE	/* Go to start of next cache line */
	bdnz	flushLoop
	sync

	rlwinm	r4,r7,0,17,15		/* still mask EE but reenable data relocation */
	mtmsr	r4
	isync

flushDone:
	
	/* Turn off the L2CR enable bit. */
	rlwinm	r3,r3,0,1,31
	
dontDisableCache:
	/* Set up the L2CR configuration bits */
	sync
	mtspr	L2CR,r3
	sync
	cmplwi	r6,0
	beq	noInval
	
	/* Perform a global invalidation */
	oris	r3,r3,0x0020
	sync
	mtspr	L2CR,r3
	sync
invalCompleteLoop:				/* Wait for the invalidation to complete */
	mfspr	r3,L2CR
	rlwinm.	r4,r3,0,31,31
	bne	invalCompleteLoop
	
	rlwinm	r3,r3,0,11,9;		/* Turn off the L2I bit */
	sync
	mtspr	L2CR,r3
	sync
	
noInval:
	/* re-enable interrupts, i.e. restore original MSR */
	mtmsr	r7					/* (no sync needed) */
	/* See if we need to enable the cache */
	cmplwi	r5,0
	beqlr
	
enableCache:
	/* Enable the cache */
	oris	r3,r3,0x8000
	mtspr	L2CR,r3
	sync
	blr
