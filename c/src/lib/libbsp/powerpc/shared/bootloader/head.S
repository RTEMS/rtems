/*
 *  head.S -- Bootloader Entry point
 *
 *  Copyright (C) 1998, 1999 Gabriel Paubert, paubert@iram.es
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include "bootldr.h"
#include <bspopts.h>

#define TEST_PPCBUG_CALLS
#undef TEST_PPCBUG_CALLS

#define FRAME_SIZE 32
#define LOCK_CACHES (HID0_DLOCK | HID0_ILOCK)
#define INVL_CACHES (HID0_DCI | HID0_ICFI)
#define ENBL_CACHES (HID0_DCE | HID0_ICE)

#ifndef qemu
#define USE_PPCBUG
#endif

#define PRINT_CHAR(c)		\
	addi	r20,r3,0	; \
	li	r3,c		; \
	li	r10,0x20	; \
	sc			; \
	addi	r3,r20,0	; \
	li	r10,0x26	; \
	sc

#define MONITOR_ENTER			\
	mfmsr	r10		;	\
	ori	r10,r10,MSR_IP	;	\
	mtmsr	r10		;	\
	li	r10,0x63	;	\
	sc

	START_GOT
	GOT_ENTRY(_GOT2_TABLE_)
	GOT_ENTRY(_FIXUP_TABLE_)
	GOT_ENTRY(.bss)
	GOT_ENTRY(codemove)
	GOT_ENTRY(0)
	GOT_ENTRY(__bd)
	GOT_ENTRY(moved)
	GOT_ENTRY(_binary_rtems_gz_start)
 	GOT_ENTRY(_binary_initrd_gz_start)
 	GOT_ENTRY(_binary_initrd_gz_end)
#ifdef TEST_PPCBUG_CALLS
	GOT_ENTRY(banner_start)
	GOT_ENTRY(banner_end)
#endif
#ifdef USE_PPCBUG
	GOT_ENTRY(nioc_reset_packet)
#endif
	END_GOT
	.globl	start
	.type	start,@function

/* Point the stack into the PreP partition header in the x86 reserved
 * code area, so that simple C routines can be called.
 */
start:
#if defined(USE_PPCBUG) && defined(DEBUG) && defined(REENTER_MONITOR)
	MONITOR_ENTER
#endif
	bl	1f
1:	mflr	r1
	li	r0,0
	stwu	r0,start-1b-0x400+0x1b0-FRAME_SIZE(r1)
	stmw	r26,FRAME_SIZE-24(r1)
	GET_GOT
	mfmsr	r28			/* Turn off interrupts */
	ori	r0,r28,MSR_EE
	xori	r0,r0,MSR_EE
	mtmsr	r0

/* Enable the caches, from now on cr2.eq set means processor is 601 */

	mfpvr	r0
	mfspr	r29,HID0
	srwi	r0,r0,16
	cmplwi	cr2,r0,1
	beq	2,2f

/*
 * commented out, 11/7/2002, gregm.  This instruction sequence seems to
 * be pathological on the 603e.
 *

#ifndef USE_PPCBUG
	ori	r0,r29,ENBL_CACHES|INVL_CACHES|LOCK_CACHES
	xori	r0,r0,INVL_CACHES|LOCK_CACHES
	sync
	isync
	mtspr	HID0,r0
#endif
*/

2:	bl	reloc

/* save all the parameters and the orginal msr/hid0/r31 */
	lwz	bd,GOT(__bd)
	stw	r3,0(bd)
	stw	r4,4(bd)
	stw	r5,8(bd)
	stw	r6,12(bd)
	stw	r7,16(bd)
	stw	r8,20(bd)
	stw	r9,24(bd)
	stw	r10,28(bd)
	stw	r28,o_msr(bd)
	stw	r29,o_hid0(bd)
	stw	r31,o_r31(bd)

#ifdef USE_PPCBUG
/* Stop the network interface - otherwise, memory can get
 * corrupted by the IF DMAing data into its old buffers or
 * by writing descriptors...
 */
	lwz r3,GOT(nioc_reset_packet)
	li  r10, 0x1d /* .NETCTRL */
	sc
#endif

/* Call the routine to fill boot_data structure from residual data.
 * And to find where the code has to be moved.
 */
	lis	r3,__size@sectoff@ha
	addi	r3,r3,__size@sectoff@l
	bl	early_setup

/* Now we need to relocate ourselves, where we are told to. First put a
 * copy of the codemove routine to some place in memory.
 * (which may be where the 0x41 partition was loaded, so size is critical).
 */
	lwz	r4,GOT(codemove)
	li	r5,_size_codemove
	lwz	r3,mover(bd)
	lwz	r6,cache_lsize(bd)

	bl	codemove

	mtctr	r3		# Where the temporary codemove is.
	lwz	r3,image(bd)
	lis	r5,_edata@sectoff@ha
	lwz	r4,GOT(0)	# Our own address
	addi	r5,r5,_edata@sectoff@l
	lwz	r6,cache_lsize(bd)
	lwz	r8,GOT(moved)

	sub	r7,r3,r4	# Difference to adjust pointers.
	add	r8,r8,r7
	add	r30,r30,r7
	add	bd,bd,r7

/* Call the copy routine but return to the new area. */

	mtlr	r8		# for the return address
	bctr			# returns to the moved instruction

/* Establish the new top stack frame. */
moved:	lwz	r1,stack(bd)
	li	r0,0
	stwu	r0,-16(r1)

/* relocate again */
	bl	reloc
/* Clear all of BSS */
	lwz	r10,GOT(.bss)
	li	r0,__bss_words@sectoff@l
	subi	r10,r10,4
	cmpwi	r0,0
	mtctr	r0
	li	r0,0
	beq	4f
3:	stwu	r0,4(r10)
	bdnz	3b

/* Final memory initialization. First switch to unmapped mode
 * in case the FW had set the MMU on, and flush the TLB to avoid
 * stale entries from interfering. No I/O access is allowed
 * during this time!
 */
4:
#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('M')
#endif
	bl	MMUoff

#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('B')
#endif
	bl	flush_tlb

/* Some firmware versions leave stale values in the BATs, it's time
 * to invalidate them to avoid interferences with our own mappings.
 * But the 601 valid bit is in the BATL (IBAT only) and others are in
 * the [ID]BATU. Bloat, bloat.. fortunately thrown away later.
 */
#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('T')
#endif
	li	r3,0
	beq	cr2,5f
	mtdbatu 0,r3
	mtdbatu 1,r3
	mtdbatu 2,r3
	mtdbatu 3,r3
5:	mtibatu 0,r3
	mtibatl	0,r3
	mtibatu 1,r3
	mtibatl 1,r3
	mtibatu 2,r3
	mtibatl 2,r3
	mtibatu 3,r3
	mtibatl 3,r3
	lis	r3,__size@sectoff@ha
	addi	r3,r3,__size@sectoff@l
	sync				# We are going to touch SDR1 !
#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('i')
#endif
	bl	mm_init

#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('M')
#endif
	bl	MMUon

/* Now we are mapped and can perform I/O if we want */
#ifdef TEST_PPCBUG_CALLS
/* Experience seems to show that PPCBug can only be called with the
 * data cache disabled and with MMU disabled. Bummer.
 */
	li	r10,0x22		# .OUTLN
	lwz	r3,GOT(banner_start)
	lwz	r4,GOT(banner_end)
	sc
#endif
#if defined(USE_PPCBUG) && defined(DEBUG)
	PRINT_CHAR('H')
#endif
	bl	setup_hw
	lwz	r4,GOT(_binary_rtems_gz_start)
	lis	r5,_rtems_gz_size@sectoff@ha
	lwz	r6,GOT(_binary_initrd_gz_start)
	lis	r3,_rtems_size@sectoff@ha
	lwz	r7,GOT(_binary_initrd_gz_end)
	addi	r5,r5,_rtems_gz_size@sectoff@l
	addi	r3,r3,_rtems_size@sectoff@l
	sub	r7,r7,r6
	bl	decompress_kernel

/* Back here we are unmapped and we start the kernel, passing up to eight
 * parameters just in case, only r3 to r7 used for now. Flush the tlb so
 * that the loaded image starts in a clean state.
 */
	bl	flush_tlb
	lwz	r3,0(bd)
	lwz	r4,4(bd)
	lwz	r5,8(bd)
	lwz	r6,12(bd)
	lwz	r7,16(bd)
	lwz	r8,20(bd)
	lwz	r9,24(bd)
	lwz	r10,28(bd)

	lwz	r30,0(0)
	mtctr	r30
/*
 *	Linux code again
 *
	lis	r30,0xdeadc0de@ha
	addi	r30,r30,0xdeadc0de@l
	stw	r30,0(0)
	li	r30,0
*/
	dcbst	0,r30	/* Make sure it's in memory ! */

/* We just flash invalidate and disable the dcache, unless it's a 601,
 * critical areas have been flushed and we don't care about the stack
 * and other scratch areas.
 */
	beq	cr2,1f
	mfspr	r0,HID0
	ori	r0,r0,HID0_DCI|HID0_DCE
	sync
	mtspr	HID0,r0
	xori	r0,r0,HID0_DCI|HID0_DCE
	mtspr	HID0,r0

/* Provisional return to FW, works for PPCBug */
#if 0 && defined(REENTER_MONITOR)
	MONITOR_ENTER
#else
1:	bctr
#endif

/* relocation function, r30 must point to got2+0x8000 */
reloc:
/* Adjust got2 pointers, no need to check for 0, this code already puts
 * a few entries in the table.
 */
	li	r0,__got2_entries@sectoff@l
	la	r12,GOT(_GOT2_TABLE_)
	lwz	r11,GOT(_GOT2_TABLE_)
	mtctr	r0
	sub	r11,r12,r11
	addi	r12,r12,-4
1:	lwzu	r0,4(r12)
	add	r0,r0,r11
	stw	r0,0(r12)
	bdnz	1b

/* Now adjust the fixups and the pointers to the fixups in case we need
 * to move ourselves again.
 */
2:	li	r0,__fixup_entries@sectoff@l
	lwz	r12,GOT(_FIXUP_TABLE_)
	cmpwi	r0,0
	mtctr	r0
	addi	r12,r12,-4
	beqlr
3:	lwzu	r10,4(r12)
	lwzux	r0,r10,r11
	add	r0,r0,r11
	stw	r10,0(r12)
	stw	r0,0(r10)
	bdnz	3b
	blr

/* Set the MMU on and off: code is always mapped 1:1 and does not need MMU,
 * but it does not cost so much to map it also and it catches calls through
 * NULL function pointers.
 */
	.globl  MMUon
	.type	MMUon,@function
MMUon:	blr
	nop

/*
	mfmsr	r0
	ori	r0,r0,MSR_IR|MSR_DR|MSR_IP
	mflr	r11
	xori	r0,r0,MSR_IP
	mtsrr0	r11
	mtsrr1	r0
	rfi
*/
	.globl  MMUoff
	.type	MMUoff,@function
MMUoff:	blr
	nop

/*
	mfmsr	r0
	ori	r0,r0,MSR_IR|MSR_DR|MSR_IP
	mflr	r11
	xori	r0,r0,MSR_IR|MSR_DR
	mtsrr0	r11
	mtsrr1	r0
	rfi
*/

/* Due to the PPC architecture (and according to the specifications), a
 * series of tlbie which goes through a whole 256 MB segment always flushes
 * the whole TLB. This is obviously overkill and slow, but who cares ?
 * It takes about 1 ms on a 200 MHz 603e and works even if residual data
 * get the number of TLB entries wrong.
 */
flush_tlb:
	lis	r11,0x1000
1:	addic.  r11,r11,-0x1000
	tlbie	r11
	bnl	1b
/* tlbsync is not implemented on 601, so use sync which seems to be a superset
 * of tlbsync in all cases and do not bother with CPU dependant code
 */
	sync
	blr

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
	.size	codemove,.-codemove
_size_codemove=.-codemove

	.section        ".data" # .rodata
	.align 4
#ifdef USE_PPCBUG
/* A control 'packet' for the .NETCTRL PPCBug syscall to
 * reset a network interface. Let's hope they used the
 * first one for booting!! (CLUN/DLUN == 0/0)
 * Must be 4-byte aligned...
 */
nioc_reset_packet:
	.byte	0	/* Contoller LUN                                   */
	.byte	0	/* Device LUN                                      */
	.word   0   /* status return                                   */
	.long	5	/* Command (5=RESET)                               */
	.long   0   /* Mem. Addr. for real data (unused for reset)     */
	.long   0   /* Number of bytes                                 */
	.long   0   /* Status/Control Flags (unused for reset)         */
#endif
#ifdef TEST_PPCBUG_CALLS
banner_start:
	.ascii "This message was printed by PPCBug with MMU enabled"
banner_end:
#endif
