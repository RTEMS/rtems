/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Cache initialization.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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
#if MPC55XX_CHIP_FAMILY != 567
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
