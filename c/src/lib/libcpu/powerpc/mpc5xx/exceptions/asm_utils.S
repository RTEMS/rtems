/*
 *  asm_utils.s
 *
 *  asm_utils.S,v 1.2 2002/04/18 20:55:37 joel Exp
 *
 *  Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *
 *  This file contains the low-level support for moving exception
 *  exception code to appropriate location.
 *
 * Adapted for MPC5XX Wilfried Busalski (w.busalski@lancier-monitoring.de)
 * (C) Lancier Monitoring GmbH
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/io.h>

//SPR defines
#define SPR_ICCST 	560


	.globl  codemove
codemove:
	.type	codemove,@function
/* r3 dest, r4 src, r5 length in bytes, r6 cachelinesize */
	cmplw	cr1,r3,r4
	addi	r0,r5,3
	srwi.	r0,r0,2
	beq	cr1,4f	/* In place copy is not necessary */
	beq	7f	/* Protect against 0 count */
	mtctr	r0
	bge	cr1,2f
	
	la	r8,-4(r4)
	la	r7,-4(r3)
1:	lwzu	r0,4(r8)
	stwu	r0,4(r7)	
	bdnz	1b
	b	4f

2:	slwi	r0,r0,2
	add	r8,r4,r0
	add	r7,r3,r0
3:	lwzu	r0,-4(r8)
	stwu	r0,-4(r7)
	bdnz	3b
	
/* Now flush the cache:	note that we must start from a cache aligned
 * address. Otherwise we might miss one cache line. 
 */
 
4:	lis		r0, 0x0A00		// Command Unlock All
	mtspr	SPR_ICCST, r0	// Cache Unlock ALL	                               

	lis		r0, 0x0C00		// Command Invalidate All
	mtspr	SPR_ICCST, r0	// Cache Invalidate ALL	                               

	lis		r0, 0x0200		// Command Enable All
	mtspr	SPR_ICCST, r0	// Cache Enable ALL	                               
	
7:	sync		/* Wait for all icbi to complete on bus */
	isync
	blr
