/*
 *  exception.S -- Exception handlers for early boot.
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

/* This is an improved version of the TLB interrupt handling code from
 * the 603e users manual (603eUM.pdf) downloaded from the WWW. All the
 * visible bugs have been removed. Note that many have survived in the errata
 * to the 603 user manual (603UMer.pdf).
 *
 *  This code also pays particular attention to optimization, takes into
 * account the differences between 603 and 603e, single/multiple processor
 * systems and tries to order instructions for dual dispatch in many places.
 *
 *  The optimization has been performed along two lines:
 * 1) to minimize the number of instruction cache lines needed for the most
 *    common execution paths (the ones that do not result in an exception).
 * 2) then to order the code to maximize the number of dual issue and
 *    completion opportunities without increasing the number of cache lines
 *    used in the same cases.
 *
 *  The last goal of this code is to fit inside the address range
 * assigned to the interrupt vectors: 192 instructions with fixed
 * entry points every 64 instructions.
 *
 *  Some typos have also been corrected and the Power l (lowercase L)
 * instructions replaced by lwz without comment.
 *
 *  I have attempted to describe the reasons of the order and of the choice
 * of the instructions but the comments may be hard to understand without
 * the processor manual.
 *
 *  Note that the fact that the TLB are reloaded by software in theory
 * allows tremendous flexibility, for example we could avoid setting the
 * reference bit of the PTE which will could actually not be accessed because
 * of protection violation by changing a few lines of code. However,
 * this would significantly slow down most TLB reload operations, and
 * this is the reason for which we try never to make checks which would be
 * redundant with hardware and usually indicate a bug in a program.
 *
 *  There are some inconsistencies in the documentation concerning the
 * settings of SRR1 bit 15. All recent documentations say now that it is set
 * for stores and cleared for loads. Anyway this handler never uses this bit.
 *
 *  A final remark, the rfi instruction seems to implicitly clear the
 * MSR<14> (tgpr)bit. The documentation claims that this bit is restored
 * from SRR1 by rfi, but the corresponding bit in SRR1 is the LRU way bit.
 * Anyway, the only exception which can occur while TGPR is set is a machine
 * check which would indicate an unrecoverable problem. Recent documentation
 * now says in some place that rfi clears MSR<14>.
 *
 *  TLB software load for 602/603/603e/603ev:
 *    Specific Instructions:
 *      tlbld - write the dtlb with the pte in rpa reg
 *      tlbli - write the itlb with the pte in rpa reg
 *    Specific SPRs:
 *      dmiss - address of dstream miss
 *      imiss - address of istream miss
 *      hash1 - address primary hash PTEG address
 *      hash2 - returns secondary hash PTEG address
 *      iCmp - returns the primary istream compare value
 *      dCmp - returns the primary dstream compare value
 *      rpa - the second word of pte used by tlblx
 *    Other specific resources:
 *      cr0 saved in 4 high order bits of SRR1,
 *      SRR1 bit 14 [WAY] selects TLB set to load from LRU algorithm
 *      gprs r0..r3 shadowed by the setting of MSR bit 14 [TGPR]
 *      other bits in SRR1 (unused by this handler but see earlier comments)
 *
 *    There are three basic flows corresponding to three vectors:
 *      0x1000: Instruction TLB miss,
 *      0x1100: Data TLB miss on load,
 *      0x1200: Data TLB miss on store or not dirty page
 */

/* define the following if code does not have to run on basic 603 */
/* #define USE_KEY_BIT */

/* define the following for safe multiprocessing */
/* #define MULTIPROCESSING */

/* define the following for mixed endian */
/* #define CHECK_MIXED_ENDIAN */

/* define the following if entries always have the reference bit set */
#define ASSUME_REF_SET

/* Some OS kernels may want to keep a single copy of the dirty bit in a per
 * page table. In this case writable pages are always write-protected as long
 * as they are clean, and the dirty bit set actually means that the page
 * is writable.
 */
#define DIRTY_MEANS_WRITABLE

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include "bootldr.h"

/*
 * Instruction TLB miss flow
 *   Entry at 0x1000 with the following:
 *     srr0 -> address of instruction that missed
 *     srr1 -> 0:3=cr0, 13=1 (instruction), 14=lru way, 16:31=saved MSR
 *     msr<tgpr> -> 1
 *     iMiss -> ea that missed
 *     iCmp -> the compare value for the va that missed
 *     hash1 -> pointer to first hash pteg
 *     hash2 -> pointer to second hash pteg
 *
 *   Register usage:
 *     r0 is limit address during search / scratch after
 *     r1 is pte data / error code for ISI exception when search fails
 *     r2 is pointer to pte
 *     r3 is compare value during search / scratch after
 */
/* Binutils or assembler bug ? Declaring the section executable and writable
 * generates an error message on the @fixup entries.
 */
	.section .exception,"aw"
#	.org    0x1000        # instruction TLB miss entry point
	.globl	tlb_handlers
tlb_handlers:
	.type	tlb_handlers,@function
#define ISIVec tlb_handlers-0x1000+0x400
#define DSIVec tlb_handlers-0x1000+0x300
	mfspr   r2,HASH1
	lwz     r1,0(r2)      # Start memory access as soon as possible
	mfspr   r3,ICMP       # to load the cache.
0:	la      r0,48(r2)     # Use explicit loop to avoid using ctr
1:	cmpw    r1,r3         # In theory the loop is somewhat slower
	beq-    2f            # than documentation example
	cmpw    r0,r2         # but we gain from starting cache load
	lwzu    r1,8(r2)      # earlier and using slots between load
	bne+    1b            # and comparison for other purposes.
	cmpw    r1,r3
	bne-    4f            # Secondary hash check
2:	lwz     r1,4(r2)      # Found:	load second word of PTE
	mfspr   r0,IMISS      # get miss address during load delay
#ifdef ASSUME_REF_SET
	andi.	r3,r1,8       # check for guarded memory
	bne-	5f
	mtspr	PPC_RPA,r1
	mfsrr1	r3
	tlbli	r0
#else
/* This is basically the original code from the manual. */
#	andi.   r3,r1,8       # check for guarded memory
#	bne-    5f
#	andi.   r3,r1,0x100   # check R bit ahead to help folding
/* However there is a better solution: these last three instructions can be
replaced by the following which should cause less pipeline stalls because
both tests are combined and there is a single CR rename buffer */
	extlwi  r3,r1,6,23    # Keep only RCWIMG in 6 most significant bits.
	rlwinm. r3,r3,5,0,27  # Keep only G (in sign) and R and test.
	blt-    5f            # Negative means guarded, zero R not set.
	mfsrr1  r3            # get saved cr0 bits now to dual issue
	ori     r1,r1,0x100
	mtspr   PPC_RPA,r1
	tlbli   r0
/* Do not update PTE if R bit already set, this will save one cache line
writeback at a later time, and avoid even more bus traffic in
multiprocessing systems, when several processors access the same PTEGs.
We also hope that the reference bit will be already set. */
	bne+    3f
#ifdef MULTIPROCESSING
	srwi    r1,r1,8       # get byte 7 of pte
	stb     r1,+6(r2)     # update page table
#else
	sth     r1,+6(r2)     # update page table
#endif
#endif
3:	mtcrf   0x80,r3       # restore CR0
	rfi                   # return to executing program

/* The preceding code is 20 to 25 instructions long, which occupies
3 or 4 cache lines. */
4:	andi.   r0,r3,0x0040  # see if we have done second hash
	lis     r1,0x4000     # set up error code in case next branch taken
	bne-    6f            # speculatively issue the following
	mfspr   r2,HASH2      # get the second pointer
	ori     r3,r3,0x0040  # change the compare value
	lwz     r1,0(r2)      # load first entry
	b       0b            # and go back to main loop
/* We are now at 27 to 32 instructions, using 3 or 4 cache lines for all
cases in which the TLB is successfully loaded. */

/* Guarded memory protection violation: synthesize an ISI exception. */
5:	lis     r1,0x1000     # set srr1<3>=1 to flag guard violation
/* Entry Not Found branches here with r1 correctly set. */
6:	mfsrr1  r3
	mfmsr   r0
	insrwi  r1,r3,16,16   # build srr1 for ISI exception
	mtsrr1  r1            # set srr1
/* It seems few people have realized rlwinm can be used to clear a bit or
a field of contiguous bits in a register by setting mask_begin>mask_end. */
	rlwinm  r0,r0,0,15,13 # clear the msr<tgpr> bit
	mtcrf   0x80, r3      # restore CR0
	mtmsr   r0            # flip back to the native gprs
	isync                 # Required from 602 doc!
	b       ISIVec        # go to instruction access exception
/* Up to now there are 37 to 42 instructions so at least 20 could be
inserted for complex cases or for statistics recording. */


/*
  Data TLB miss on load flow
    Entry at 0x1100 with the following:
      srr0 -> address of instruction that caused the miss
      srr1 -> 0:3=cr0, 13=0 (data), 14=lru way, 15=0, 16:31=saved MSR
      msr<tgpr> -> 1
      dMiss -> ea that missed
      dCmp -> the compare value for the va that missed
      hash1 -> pointer to first hash pteg
      hash2 -> pointer to second hash pteg

    Register usage:
      r0 is limit address during search / scratch after
      r1 is pte data / error code for DSI exception when search fails
      r2 is pointer to pte
      r3 is compare value during search / scratch after
*/
	.org	tlb_handlers+0x100
	mfspr   r2,HASH1
	lwz     r1,0(r2)      # Start memory access as soon as possible
	mfspr   r3,DCMP       # to load the cache.
0:	la      r0,48(r2)     # Use explicit loop to avoid using ctr
1:	cmpw    r1,r3         # In theory the loop is somewhat slower
	beq-    2f            # than documentation example
	cmpw    r0,r2         # but we gain from starting cache load
	lwzu    r1,8(r2)      # earlier and using slots between load
	bne+    1b            # and comparison for other purposes.
	cmpw    r1,r3
	bne-    4f            # Secondary hash check
2:	lwz     r1,4(r2)      # Found:	load second word of PTE
	mfspr   r0,DMISS      # get miss address during load delay
#ifdef ASSUME_REF_SET
	mtspr	PPC_RPA,r1
	mfsrr1	r3
	tlbld	r0
#else
	andi.   r3,r1,0x100   # check R bit ahead to help folding
	mfsrr1  r3            # get saved cr0 bits now to dual issue
	ori     r1,r1,0x100
	mtspr   PPC_RPA,r1
	tlbld   r0
/* Do not update PTE if R bit already set, this will save one cache line
writeback at a later time, and avoid even more bus traffic in
multiprocessing systems, when several processors access the same PTEGs.
We also hope that the reference bit will be already set. */
	bne+    3f
#ifdef MULTIPROCESSING
	srwi    r1,r1,8       # get byte 7 of pte
	stb     r1,+6(r2)     # update page table
#else
	sth     r1,+6(r2)     # update page table
#endif
#endif
3:	mtcrf   0x80,r3       # restore CR0
	rfi                   # return to executing program

/* The preceding code is 18 to 23 instructions long, which occupies
3 cache lines. */
4:	andi.   r0,r3,0x0040  # see if we have done second hash
	lis     r1,0x4000     # set up error code in case next branch taken
	bne-    9f            # speculatively issue the following
	mfspr   r2,HASH2      # get the second pointer
	ori     r3,r3,0x0040  # change the compare value
	lwz     r1,0(r2)      # load first entry asap
	b       0b            # and go back to main loop
/* We are now at 25 to 30 instructions, using 3 or 4 cache lines for all
cases in which the TLB is successfully loaded. */


/*
  Data TLB miss on store or not dirty page flow
    Entry at 0x1200 with the following:
      srr0 -> address of instruction that caused the miss
      srr1 -> 0:3=cr0, 13=0 (data), 14=lru way, 15=1, 16:31=saved MSR
      msr<tgpr> -> 1
      dMiss -> ea that missed
      dCmp -> the compare value for the va that missed
      hash1 -> pointer to first hash pteg
      hash2 -> pointer to second hash pteg

    Register usage:
      r0 is limit address during search / scratch after
      r1 is pte data / error code for DSI exception when search fails
      r2 is pointer to pte
      r3 is compare value during search / scratch after
*/
	.org	tlb_handlers+0x200
	mfspr   r2,HASH1
	lwz     r1,0(r2)      # Start memory access as soon as possible
	mfspr   r3,DCMP       # to load the cache.
0:	la      r0,48(r2)     # Use explicit loop to avoid using ctr
1:	cmpw    r1,r3         # In theory the loop is somewhat slower
	beq-    2f            # than documentation example
	cmpw    r0,r2         # but we gain from starting cache load
	lwzu    r1,8(r2)      # earlier and using slots between load
	bne+    1b            # and comparison for other purposes.
	cmpw    r1,r3
	bne-    4f            # Secondary hash check
2:	lwz     r1,4(r2)      # Found:	load second word of PTE
	mfspr   r0,DMISS      # get miss address during load delay
/* We could simply set the C bit and then rely on hardware to flag protection
violations. This raises the problem that a page which actually has not been
modified may be marked as dirty and violates the OEA model for guaranteed
bit settings (table 5-8 of 603eUM.pdf). This can have harmful consequences
on operating system memory management routines, and play havoc with copy on
write schemes. So the protection check is ABSOLUTELY necessary. */
	andi.   r3,r1,0x80    # check C bit
	beq-    5f            # if (C==0) go to check protection
3:	mfsrr1  r3            # get the saved cr0 bits
	mtspr   PPC_RPA,r1        # set the pte
	tlbld   r0            # load the dtlb
	mtcrf   0x80,r3       # restore CR0
	rfi                   # return to executing program
/* The preceding code is 20 instructions long, which occupy
3 cache lines. */
4:	andi.   r0,r3,0x0040  # see if we have done second hash
	lis     r1,0x4200     # set up error code in case next branch taken
	bne-    9f            # speculatively issue the following
	mfspr   r2,HASH2      # get the second pointer
	ori     r3,r3,0x0040  # change the compare value
	lwz     r1,0(r2)      # load first entry asap
	b       0b            # and go back to main loop
/* We are now at 27 instructions, using 3 or 4 cache lines for all
cases in which the TLB C bit is already set. */

#ifdef DIRTY_MEANS_WRITABLE
5:	lis     r1,0x0A00     # protection violation on store
#else
/*
  Entry found and C==0: check protection before setting C:
    Register usage:
      r0 is dMiss register
      r1 is PTE entry (to be copied to RPA if success)
      r2 is pointer to pte
      r3 is trashed

    For the 603e, the key bit in SRR1 helps to decide whether there is a
  protection violation. However the way the check is done in the manual is
  not very efficient. The code shown here works as well for 603 and 603e and
  is much more efficient for the 603 and comparable to the manual example
  for 603e. This code however has quite a bad structure due to the fact it
  has been reordered to speed up the most common cases.
*/
/* The first of the following two instructions could be replaced by
andi. r3,r1,3 but it would compete with cmplwi for cr0 resource. */
5:	clrlwi  r3,r1,30      # Extract two low order bits
	cmplwi  r3,2          # Test for PP=10
	bne-    7f            # assume fallthrough is more frequent
6:	ori     r1,r1,0x180   # set referenced and changed bit
	sth     r1,6(r2)      # update page table
	b       3b            # and finish loading TLB
/* We are now at 33 instructions, using 5 cache lines. */
7:	bgt-    8f            # if PP=11 then DSI protection exception
/* This code only works if key bit is present (602/603e/603ev) */
#ifdef USE_KEY_BIT
	mfsrr1  r3            # get the KEY bit and test it
	andis.  r3,r3,0x0008
	beq     6b            # default prediction taken, truly better ?
#else
/* This code is for all 602 and 603 family models: */
	mfsrr1  r3            # Here the trick is to use the MSR PR bit as a
	mfsrin  r0,r0         # shift count for an rlwnm. instruction which
	extrwi  r3,r3,1,17    # extracts and tests the correct key bit from
	rlwnm.  r3,r0,r3,1,1  # the segment register. RISC they said...
	mfspr   r0,DMISS      # Restore fault address to r0
	beq     6b            # if 0 load tlb else protection fault
#endif
/* We are now at 40 instructions, (37 if using key bit), using 5 cache
lines in all cases in which the C bit is successfully set */
8:	lis     r1,0x0A00     # protection violation on store
#endif /* DIRTY_IS_WRITABLE */
/* PTE entry not found branch here with DSISR code in r1 */
9:	mfsrr1  r3
	mtdsisr r1
	clrlwi  r2,r3,16      # set up srr1 for DSI exception
	mfmsr   r0
/* I have some doubts about the usefulness of the xori instruction in
mixed or pure little-endian environment. The address is in the same
doubleword, hence in the same protection domain and performing an exclusive
or with 7 is only valid for byte accesses. */
#ifdef CHECK_MIXED_ENDIAN
	andi.   r1,r2,1       # test LE bit ahead to help folding
#endif
	mtsrr1  r2
	rlwinm  r0,r0,0,15,13 # clear the msr<tgpr> bit
	mfspr   r1,DMISS      # get miss address
#ifdef CHECK_MIXED_ENDIAN
	beq     1f            # if little endian then:
	xori    r1,r1,0x07    # de-mung the data address
1:
#endif
	mtdar   r1            # put in dar
	mtcrf   0x80,r3       # restore CR0
	mtmsr   r0            # flip back to the native gprs
	isync                 # required from 602 manual
	b       DSIVec        # branch to DSI exception
/* We are now between 50 and 56 instructions. Close to the limit
but should be sufficient in case bugs are found. */
/* Altogether the three handlers occupy 128 instructions in the worst
case, 64 instructions could still be added (non contiguously). */
	.org	tlb_handlers+0x300
	.globl	_handler_glue
_handler_glue:
/* Entry code for exceptions: DSI (0x300), ISI(0x400), alignment(0x600) and
 * traps(0x700). In theory it is not necessary to save and restore r13 and all
 * higher numbered registers, but it is done because it allowed to call the
 * firmware (PPCBug) for debugging in the very first stages when writing the
 * bootloader.
 */
	stwu	r1,-160(r1)
	stw	r0,save_r(0)
	mflr	r0
	stmw	r2,save_r(2)
	bl	0f
0:	mfctr	r4
	stw	r0,save_lr
	mflr	r9		/* Interrupt vector + few instructions */
	la	r10,160(r1)
	stw	r4,save_ctr
	mfcr	r5
	lwz	r8,2f-0b(r9)
	mfxer	r6
	stw	r5,save_cr
	mtctr	r8
	stw	r6,save_xer
	mfsrr0	r7
	stw	r10,save_r(1)
	mfsrr1	r8
	stw	r7,save_nip
	la	r4,8(r1)
	lwz	r13,1f-0b(r9)
	rlwinm	r3,r9,24,0x3f	/* Interrupt vector >> 8 */
	stw	r8,save_msr
	bctrl

	lwz	r7,save_msr
	lwz	r6,save_nip
	mtsrr1	r7
	lwz	r5,save_xer
	mtsrr0	r6
	lwz	r4,save_ctr
	mtxer	r5
	lwz	r3,save_lr
	mtctr	r4
	lwz	r0,save_cr
	mtlr	r3
	lmw	r2,save_r(2)
	mtcr	r0
	lwz	r0,save_r(0)
	la	r1,160(r1)
	rfi
1:	.long	(__bd)@fixup
2:	.long	(_handler)@fixup
	.section .fixup,"aw"
	.align	2
	.long 1b, 2b
	.previous
