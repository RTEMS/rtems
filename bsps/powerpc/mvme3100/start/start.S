/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME3100
 *
 * @brief RTEMS entry point
 */

/*
 * Copyright (C) 2007 Till Straumann <strauman@slac.stanford.edu>
 * Copyright (C) 1999 Eric Valette <eric.valette@free.fr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/asm.h>
#include <libcpu/powerpc-utility.h>

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

	/*
	 * Initialize start stack.  The stacks are statically allocated and
	 * properly aligned.
	 */
	LA	r1, _ISR_Stack_area_end
	subi	r1, r1, PPC_DEFAULT_CACHE_LINE_SIZE
	li	r0, 0
	stw	r0, 0(r1)

	/*
	 * We are now in a environment that is totally independent from
	 * bootloader setup.
	 */
    /* pass result of 'save_boot_params' to 'boot_card' in R3 */
	bl	boot_card
	/* point of no return: reset board here ? */
