/*
 * Mini-loader for the SVGM BSP.
 *
 * Author: Till Straumann, 10/2001 <strauman@slac.stanford.edu>
 *
 * Some ideas are borrowed from the powerpc/shared/bootloader
 * by
 *  Copyright (C) 1998, 1999 Gabriel Paubert, paubert@iram.es
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 * The SMON firmware is unable to load the RTEMS image below
 * 0x2000 (I believe their stack is growing below 0x1000).
 *
 * The code provided by this file is responsible for the performing
 * the following steps:
 *
 *  1) Save commandline parameters to an area that is
 *       a) not covered by the downloaded image
 *       b) will not be overwritten by the moved image
 *          nor the final BSS segment (rtems clears BSS
 *          before saving the command line).
 *  2) Move the entire image (including this very file) to
 *     its final location starting at 0x0000.
 *     It is important to note that _NO_STACK_ is available
 *     during this step. Also, there is probably no return to
 *     SMON because relocating RTEMS will destroy vital SMON
 *     data (such as its stack).
 *  3) Flush the cache to make sure the relocated image is actually
 *     in memory.
 *  4) setup RTEMS environment (initial register values), most
 *     notably an initial STACK. The initial stack may be small and
 *     is used by RTEMS only at a very early stage.
 *     A safe place for the stack seems to be the 00..0x7f area.
 *     NOTE: we should respect the MAILBOX area 0x80..0xff!
 *  5) switch the MMU off (because that's what RTEMS is expecting
 *     it to be at startup).
 *  6) fire up rtems...
 *
 *
 *  Calling convention:
 *     R1: SMON SP
 *     R3: command line string start
 *     R4: command line string end + 1
 *     R5: where SMON put the image
 *         if R5 is 0, the preloader will use its entry point
 *         as the image starting address.
 *         See NOTE below.
 *     R6: end of the image (i.e. R6-R5 is the image length)
 *         if R6 is 0, _edata will be used as the image length
 *         See NOTE below.
 *
 *     NOTE: if the symbol DONT_USE_R5_ENTRY is defined,
 *         R5/R6 are never used and the necessary parameters are
 *         determined at runtime (R5) / linkage (R6) [_edata]
 *
 *  ASSUMPTIONS:
 *    The code RELIES on the assumption that the image will be
 *    moved DOWNWARDS in memory and that the this loader is
 *    prepended to the image, i.e. it is safe to do
 *        codemove(codemove,0,codemove_end - codemove);
 *        (*0)(codemove_end, codemove_end-codemove, __rtems_end-codemove_end);
 *    where codemove(from, to, nbytes) is defined as
 *        codemove(from, to, nbytes) { while (nbytes--) *(to++)=*(from++); }
 *    Implicit to these assumptions is the assumption that the destination
 *    address is cache block aligned.
 *    Furthermore, the byte count is assumed to be a multiple
 *    of four
 *
 */
#if 0
#include <rtems/score/powerpc.h>
#else
#ifndef PPC_CACHE_ALIGNMENT
#define PPC_CACHE_ALIGNMENT	32
#endif
#endif

#include <rtems/score/cpu.h>
#include <rtems/asm.h>

/* Note that major modifications may be needed
 * if DESTINATION_ADDR is not 0
 */
#define KERNELBASE			0x0
#define INITIAL_STACK		0x70					/* 16-byte aligned */
#define CACHE_LINE_SIZE		PPC_CACHE_ALIGNMENT 	/* autodetect doesn't work, see below */
#define	ASSUME_RTEMS_INSTALLS_VECTORS				/* assume we need not load vectors */
#define DONT_USE_R5_ENTRY							/* always dynamically determine the address we're running from */

	/* put this into its own section which we want to
	 * be loaded at the very beginning. We should probably
	 * not use more than 255 bytes.
	 */
	PUBLIC_VAR(__rtems_start)
	PUBLIC_VAR(__rtems_entry_point)
	PUBLIC_VAR(__rtems_end)
	.section .entry_point_section,"awx",@progbits
preload:
	/* find out where we are */
	bl	here
here:
	xor		r0,r0,r0
	mtmsr	r0	/* clear MSR to known state */
	mflr	r5
	addi	r5,r5,-(here-preload)
	lis		r27,_edata@h
	ori		r27,r27,_edata@l

	/* at this point the register contents are
	 * R3:  command line start
	 * R4:  R3 + command line length
	 * R5:  address we are running from / loaded to
	 * R27: image end
	 */

	/* save command line start */
	mr		r6, r3
	/* save the command line parameters if they are to be overwritten */
	sub.	r17, r4, r3		/* string length */
	ble		leaveparms		/* <=0 -> no parameters */
	/* copy has to be out of the way of the bss; therefore we must
	 * put the string out of the way of both, the current end of
	 * the image (without bss) AND the end of the loaded image
	 * (including bss):
	 * |......image.........|  downloaded image
	 * |image_bss...........|  loaded image with bss appended
	 *
	 *             ^ safe place for string
	 *
	 * the alternative scenario looks like this:
	 * |..image.............|  downloaded image
	 * |image_bss...........|  loaded image with bss appended
	 *           ^ safe place for string
	 */
	lis		r18, __rtems_end+0x10000@h	/* round up, save one instruction */
	add		r16, r5, r27	/* image end + 1 */
	cmpw	r16, r18
	bge		ishighenough
	mr		r16,r18			/* __rtems_end is higher than the image end
							 * (without bss)
							 */
ishighenough:
	cmpw	r16, r3 	    /* destination start > current string start ? */
	ble		leaveparms		/* string already after dst, leave it */
	/* copy string from the last byte downwards */
	add		r6, r16, r17	/* last byte of destination + 1 */
	mtctr	r17
1:
	lbzu	r3, -1(r4)
	stbu	r3,	-1(r6)
	bdnz	1b
leaveparms:
	add		r7, r6, r17		/* destination + strlen */

#ifndef CACHE_LINE_SIZE
	/* Oh well, SMON has inhibited the cache, so this
	 * nice routine doesn't work...
	 */
	/* figure out the cache line size */
	li		r16, 0x80
	cmpw	r5, r16			/* 'from' must be > 0x80 */
	blt		panic

1:	/* store some arbitrary, nonzero stuff in 0..0x7c */
	stwu	r16,-4(r16)
	cmpwi	r16,0
	bne		1b
	dcbz	0,r16			/* zero out one cache line */
	subi	r16,r16,4
2:	lwzu	r0,4(r16)		/* search for a non-zero word */
	cmpwi	r0,0
	beq		2b
	/* OK, r16 now hold the size of a cache line in bytes */
#else
	li		r16,CACHE_LINE_SIZE
#endif

    lis		r3,preload@h
	ori		r3,r3,preload@l
	mr		r4,r5			/* from-addr */
	li		r5,_preload_size/* this is never > 16k */
	/* now move ourselves to the link address ('preload').
	 * We set up the LR, so domove() 'returns' to the
	 * relocated copy
	 */
	lis		r0,return_here@h
	ori		r0,r0,return_here@l
	mtlr	r0
	b		domove			/* move the preloader itself */
return_here:
	/* now we move the entire rest of the image */
#ifdef ASSUME_RTEMS_INSTALLS_VECTORS
	lis		r3,__rtems_start@h
	ori		r3,r3,__rtems_start@l
	lis		r0,preload@h	/* calculate/adjust from address */
	ori		r0,r0,preload@l
	sub		r0,r3,r0
	add		r4,r4,r0
	sub		r5,r27,r3
#else
	add		r3,r3,r5		/* add preloader size to destination */
	add		r4,r4,r5		/* and source addresses	*/
	sub		r5,r27,r5		/* length of the remaining rest */
#endif
	bl		domove
	/* OK, now everything should be in place.
     * we are ready to start...
	 */

	/* setup initial stack for rtems early boot */
	li		r1,INITIAL_STACK
	/* tag TOS with a NULL pointer (for stack trace) */
	li      r0, 0
	stw     r0, 0(r1)
	/* disable the MMU and fire up rtems */
	mfmsr	r0
	ori 	r0,r0,MSR_IR|MSR_DR|MSR_IP|MSR_ME
	xori	r0,r0,MSR_IR|MSR_DR
	mtsrr1	r0
	lis		r0,__rtems_entry_point@h
	ori		r0,r0,__rtems_entry_point@l
	mtsrr0	r0
	/* R6: start of command line */
	/* R7: end of command line +1 */
	rfi

    /* domove(to, from, nbytes):
     *
     * move a R5 bytes from R4 to R3 and flush
     * the caches for the destination memory
     * region. R16 provides the cache line size.
	 * DESTROYS: R0, R17, R18, CTR, CR
     */
domove:
	addi	r0,r5,3			/* convert to word count */
	srwi.	r0,r0,2
	beq	3f					/* nothing to do */
	cmpw	r3,r4			/* from == to ?  */
	beq 3f
	mtctr	r0
	la		r18,-4(r4)
	la		r17,-4(r3)
1:	lwzu	r0,4(r18)
	stwu	r0,4(r17)
	bdnz	1b				/* move data */
	/* now, we must flush the destination cache region */
#ifndef CACHE_LINE_SIZE
	cmpwi	r16,0
	beq		3f				/* nothing to do */
#endif
#if defined(CACHE_LINE_SIZE) && CACHE_LINE_SIZE > 0
	add		r17,r3,r5		/* target end pointer */
	subi	r0,r16,1
	add		r17,r17,r0
	andc	r17,r17,r0		/* cache aligned target end pointer */
	mr		r18,r3
2:	cmpw	r18,r17
	dcbst	0,r18			/* write out data cache line */
	icbi	0,r18			/* invalidate corresponding i-cache line */
	add		r18,r18,r16
	blt		2b
	sync					/* make sure data is written back */
	isync					/* invalidate possibly preloaded instructions */
#endif
3:
	blr

#if !defined(CACHE_LINE_SIZE)
panic:
	li		r10,0x63
	mfmsr	r0
	ori		r0,r0,MSR_IP
	mtmsr	r0
	sc
#endif

/* DONT PUT ANY CODE BELOW HERE */
_preload_size = . - preload
