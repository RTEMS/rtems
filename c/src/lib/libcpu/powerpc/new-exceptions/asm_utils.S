/*
 *  asm_utils.s
 *
 *  Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *
 *  This file contains the low-level support for moving exception
 *  exception code to appropriate location.
 *
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

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
4:	cmpwi	r6,0
	add	r5,r3,r5
	beq	7f	/* Always flush prefetch queue in any case */
	subi	r0,r6,1
	andc	r3,r3,r0
	mr	r4,r3
5:	cmplw	r4,r5
	dcbst	0,r4
	add	r4,r4,r6
	blt	5b
	sync		/* Wait for all dcbst to complete on bus */
	mr	r4,r3
6:	cmplw	r4,r5
	icbi	0,r4
	add	r4,r4,r6
	blt	6b
7:	sync		/* Wait for all icbi to complete on bus */
	isync
	blr
