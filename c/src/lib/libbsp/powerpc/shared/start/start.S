/*
 *  start.S :	  RTEMS entry point
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/io.h>
#include <libcpu/bat.h>
#include <bspopts.h>

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
 *   The Prep boot loader insure that the MMU is currently off...
 *
 */

	mr	r31,r3			/* save parameters */
	mr	r30,r4
	mr	r29,r5
	mr	r28,r6
	mr	r27,r7

#ifdef __ALTIVEC__
	/* enable altivec; gcc may use it! */
	mfmsr r0
	oris  r0, r0, (1<<(31-16-6))
	mtmsr r0
	/*
	 * set vscr and vrsave to known values
	 */
	li    r0, 0
	mtvrsave r0
	vxor   0,0,0
	mtvscr 0
#endif

	/*
	 * Make sure we have nothing in BATS and TLB
	 */
	bl	CPU_clear_bats_early
	bl	flush_tlbs
/*
 * Use the first pair of BAT registers to map the 1st 256MB
 * of RAM to KERNELBASE.
 */
	lis	r11,KERNELBASE@h
/* set up BAT registers for 604 */
	ori	r11,r11,0x1ffe
	li	r8,2			/* R/W access */
	isync
	mtspr	DBAT0L,r8		/* N.B. 6xx (not 601) have valid */
	mtspr	DBAT0U,r11		/* bit in upper BAT register */
	mtspr	IBAT0L,r8
	mtspr	IBAT0U,r11
	isync

/*
 * we now have the 1st 256M of ram mapped with the bats. We are still
 * running on the bootloader stack and cannot switch to an RTEMS allocated
 * init stack before copying the residual data that may have been set just after
 * rtems_end address. This bug has been experienced on MVME2304. Thank to
 * Till Straumann <strauman@SLAC.Stanford.EDU> for hunting it and suggesting
 * the appropriate code.
 */

enter_C_code:
	bl	MMUon
	bl      __eabi	/* setup EABI and SYSV environment */
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
	/*
	 * stack = &__rtems_end + 4096
	 */
	addis	r9,r0, __stack-PPC_MINIMUM_STACK_FRAME_SIZE@ha
	addi	r9,r9, __stack-PPC_MINIMUM_STACK_FRAME_SIZE@l
	/*
	 * align initial stack
	 * (we hope that the bootloader stack was 16-byte aligned
	 * or we haven't used altivec yet...)
	 */
	li   r0, (CPU_STACK_ALIGNMENT-1)
	andc r1, r9, r0
	/*
	 * Tag TOS with a NULL (terminator for stack dump)
	 */
	li   r0, 0
	stw  r0, 0(r1)
	
	/*
	 * We are now in a environment that is totally independent from
	 * bootloader setup.
	 */
    /* pass result of 'save_boot_params' to 'boot_card' in R3 */
	bl	boot_card
	bl	_return_to_ppcbug

	.globl  MMUon
	.type	MMUon,@function
MMUon:
	mfmsr	r0
	ori	r0,r0, MSR_IP | MSR_RI | MSR_IR | MSR_DR | MSR_EE | MSR_FE0 | MSR_FE1 | MSR_FP
#if (PPC_HAS_FPU == 0)
	xori	r0, r0, MSR_EE | MSR_IP | MSR_FP
#else
	xori	r0, r0, MSR_EE | MSR_IP | MSR_FE0 | MSR_FE1
#endif
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

flush_tlbs:
	lis	r20, 0x1000
1:	addic.	r20, r20, -0x1000
	tlbie	r20
	bgt	1b
	sync
	blr
