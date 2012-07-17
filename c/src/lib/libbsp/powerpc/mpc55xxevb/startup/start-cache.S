/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Cache initialization.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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
 */

#include <libcpu/powerpc-utility.h>

#include <mpc55xx/regs.h>

	.globl	mpc55xx_start_cache

	.section	".bsp_start_text", "ax"

mpc55xx_start_cache:

#ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT

	/* Load zero, CINV, and CABT) */
	li	r0, 0
	li	r3, 0x2
	li	r4, 0x4

#if defined(BSP_INSTRUCTION_CACHE_ENABLED) \
  && defined(MPC55XX_HAS_INSTRUCTION_CACHE)

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

#endif

#if (defined(BSP_DATA_CACHE_ENABLED) && defined(MPC55XX_HAS_DATA_CACHE)) \
  || ((defined(BSP_DATA_CACHE_ENABLED) \
    || defined(BSP_INSTRUCTION_CACHE_ENABLED)) \
      && defined(MPC55XX_HAS_UNIFIED_CACHE))

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

#endif

#endif /* MPC55XX_NEEDS_LOW_LEVEL_INIT */

	/* Return */
	blr
