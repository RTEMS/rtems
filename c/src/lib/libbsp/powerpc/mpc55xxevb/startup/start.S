/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Boot and system start code.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

/**
 * @defgroup mpc55xx_asm Assembler files
 *
 * @ingroup mpc55xx
 */

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

#include <mpc55xx/reg-defs.h>

#include <bsp/vectors.h>

#define HAS_CACHE (BSP_DATA_CACHE_ENABLED || BSP_INSTRUCTION_CACHE_ENABLED)

	.extern mpc55xx_fmpll_config
	.extern mpc55xx_fmpll_init
	.extern mpc55xx_flash_init
	.extern mpc55xx_early_init
	.extern bsp_start_zero
	.extern bsp_ram_start
	.extern bsp_ram_size
	.extern bsp_ram_end
	.extern __eabi
	.extern boot_card

	.globl _start
	
#ifdef MPC55XX_BOOTFLAGS
        .globl mpc55xx_bootflag_0
        .globl mpc55xx_bootflag_1
#endif

	.section ".bsp_start_text", "ax"

	/* BAM: RCHW */
	.int 0x005a0000

	/* BAM: Address of start instruction */
        .int _start

#ifdef MPC55XX_BOOTFLAGS
        /*
         * We skip over the next two boot flag words to the next 64-bit
         * aligned start address. It is 64-bit aligned to play well with
         * FLASH programming.  These boot flags can be set by debuggers
         * and emulators to customize boot.  Currently bit0 of
         * bootflag_0 means to "skip setting up the MMU", allowing
         * external MMU setup in a debugger before branching to 0x10.
         * This can be used e.g., to map FLASH into RAM.
         */
mpc55xx_bootflag_0:
        .int 0xffffffff
mpc55xx_bootflag_1:
        .int 0xffffffff
#endif

_start:

	/* Enable time base */
	li	r0, 0
	mtspr	TBWU, r0
	mtspr	TBWL, r0
	mfspr	r2, HID0
	ori	r2, r2, 0x4000
	mtspr	HID0, r2

	/* FMPLL setup */
	LWI	r3, mpc55xx_fmpll_config
	bl	mpc55xx_fmpll_init

	/* Enable branch prediction */
	LWI	r2, BUCSR_BBFI | BUCSR_BPEN
	mtspr	BUCSR, r2

	/* Set intermediate stack start to end of internal SRAM */
	LA	r1, bsp_ram_end
	subi	r1, r1, 16

	/* Enable SPE */
	mfmsr	r2
	oris	r2, r2, 0x200
	mtmsr	r2

	/* Config internal flash */
	bl	mpc55xx_flash_init

#if HAS_CACHE
	bl config_cache

	/* Enable cache in the MMU for the internal SRAM */
	LWI	r3, 0x10030000
	mtspr	FSL_EIS_MAS0, r3
	tlbre
	LWI	r4, ~0x00000008
	mfspr	r3, FSL_EIS_MAS2
	and	r3, r3, r4
	mtspr	FSL_EIS_MAS2, r3
	tlbwe
#endif

	/* Zero internal SRAM (needed to get proper ECC) */
	LA	r3, bsp_ram_start
	LA	r4, bsp_ram_size
	bl	bsp_start_zero

	/* Initialize intermediate start stack */
	li	r0, 0
	stw	r0, 0(r1)
	stw	r0, 4(r1)

	/* Do early initialization */
	bl	mpc55xx_early_init

	/* Set up EABI and SYSV environment */
	bl	__eabi

	/* Initialize start stack */
	LWI	r1, start_stack_end
	subi	r1, r1, 16
	li	r0, 0
	stw	r0, 0(r1)

	/* Clear command line */
	li	r3, 0

	/* Start RTEMS */
	bl	boot_card

	/* Spin around */
twiddle:
	b	twiddle

#if HAS_CACHE
config_cache:

	/* Load zero, CINV, and CABT) */
	li	r0, 0
	li	r3, 0x2
	li	r4, 0x4

#if MPC55XX_CHIP_TYPE / 10 == 567
start_instruction_cache_invalidation:

	/* Clear instruction cache invalidation abort */
	mtspr	FSL_EIS_L1CSR1, r0

	/* Start instruction cache invalidation */
	mtspr	FSL_EIS_L1CSR1, r3

get_instruction_cache_invalidation_status:

	/* Get instruction cache invalidation status */
	mfspr	r5, FSL_EIS_L1CSR1

	/* Check CABT */
	and.	r6, r5, r4
	bne	start_instruction_cache_invalidation

	/* Check CINV */
	and.	r6, r5, r3
	bne	get_instruction_cache_invalidation_status

	/* Save instruction cache settings */
	LWI	r6, 0x00010001
	isync
	msync
	mtspr	FSL_EIS_L1CSR1, r6
#endif /* MPC55XX_CHIP_TYPE / 10 == 567 */

start_data_cache_invalidation:

	/* Clear data cache invalidation abort */
	mtspr	FSL_EIS_L1CSR0, r0

	/* Start data cache invalidation */
	mtspr	FSL_EIS_L1CSR0, r3

get_data_cache_invalidation_status:

	/* Get data cache invalidation status */
	mfspr	r5, FSL_EIS_L1CSR0

	/* Check CABT */
	and.	r6, r5, r4
	bne	start_data_cache_invalidation

	/* Check CINV */
	and.	r6, r5, r3
	bne	get_data_cache_invalidation_status

	/* Save data cache settings */
#if MPC55XX_CHIP_TYPE / 10 != 567
	/* FIXME: CORG??? 0x00180011 */
	LWI	r6, 0x00100001
#else
	LWI	r6, 0x00190001
#endif
	isync
	msync
	mtspr	FSL_EIS_L1CSR0, r6

	/* Return */
	blr
#endif /* HAS_CACHE */

	/* Start stack area */
	.section ".bsp_rwextra", "aw", @nobits
	.align 4
	.space 4096
start_stack_end:
