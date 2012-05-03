/*
 *  start.S :	  RTEMS entry point
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified for mvme3100 by T. Straumann, 2007.
 *
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

#include <bspopts.h>

#define SYNC \
	sync; \
	isync

#define KERNELBASE	0x0

/* cannot include <bsp.h> from assembly :-( */
#ifndef BSP_8540_CCSR_BASE
#define BSP_8540_CCSR_BASE 0xe1000000
#endif

#define ERR_DISABLE_REG (BSP_8540_CCSR_BASE + 0x2e44)

	.text
	.globl	__rtems_entry_point
	.type	__rtems_entry_point,@function
__rtems_entry_point:
	mr	r31,r3
	mr	r30,r4
	mr	r29,r5
	mr	r28,r6
	mr	r27,r7
	/* disable checking for memory-select errors; motload has all TLBs
	 * mapping a possible larger area as memory (not-guarded, caching-enabled)
	 * than actual physical memory is available.
	 * In case of speculative loads this may cause 'memory-select' errors
	 * which seem to raise 'core_fault_in' (found no description in
	 * the manual but I experienced this problem).
	 * Such errors (if HID1[RFXE] is clear) may *stall* execution
	 * leading to mysterious 'hangs'.
	 * Note: enabling HID1[RFXE] at this point makes no sense since
	 *       exceptions are not configured yet. Therefore we disable
	 *       memory-select errors.
	 * Eventually (bspstart.c) we want to delete TLB entries for
	 * which no physical memory is present.
	 */
	lis r3, ERR_DISABLE_REG@ha
	lwz r4, ERR_DISABLE_REG@l(r3)
	/* disable memory-select errors */
	ori r4, r4, 1
	stw r4, ERR_DISABLE_REG@l(r3)

	/* Use MotLoad's TLB setup for now; caches are on already */
	bl  __eabi	/* setup EABI and SYSV environment */
	bl	zero_bss
	/*
	 * restore original args
	 */
	mr	r3,r31
	mr	r4,r30
	mr	r5,r29
	mr	r6,r28
	mr	r7,r27
	bl	save_boot_params
	addis	r9,r0, (__stack-PPC_MINIMUM_STACK_FRAME_SIZE)@ha
	addi	r9,r9, (__stack-PPC_MINIMUM_STACK_FRAME_SIZE)@l
	/* align down to 16-bytes */
	li  r5, (CPU_STACK_ALIGNMENT - 1)
	andc r1, r9, r5

	/* NULL ptr to back chain */
	li  r0, 0
	stw r0, 0(r1)

	/*
	 * We are now in a environment that is totally independent from
	 * bootloader setup.
	 */
    /* pass result of 'save_boot_params' to 'boot_card' in R3 */
	bl	boot_card
	/* point of no return: reset board here ? */
