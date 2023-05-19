/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Memory copy functions.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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
#include <bspopts.h>

	.section ".bsp_start_text", "ax"

/**
 * @fn int mpc55xx_copy_8( const void *src, void *dest, size_t n)
 *
 * @brief Copy @a n bytes from @a src to @a dest with 8 byte reads and writes.
 *
 * The memory areas should not overlap.  The addresses @a src and @a dest have
 * to be aligned on 8 byte boundaries.  The size @a n must be evenly divisible by 8.
 * The SPE operations @b evxor, @b evlddx and @b evstddx will be used.
 */
#if       ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))
GLOBAL_FUNCTION mpc55xx_copy_8
#endif /* ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517)) */
GLOBAL_FUNCTION mpc55xx_copy_4
	/* Loop counter = data size / 4 */
	srwi. r5, r5, 2
	beqlr
	mtctr r5
	xor   r5,r5,r5
copy_data4:
	lwzx  r6, r5, r3
	stwx  r6, r5, r4
	addi r5, r5, 4
	bdnz copy_data4

	/* Return */
	blr

#if  !((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))
/**
 * @fn int mpc55xx_copy_8( const void *src, void *dest, size_t n)
 *
 * @brief Copy @a n bytes from @a src to @a dest with 8 byte reads and writes.
 *
 * The memory areas should not overlap.  The addresses @a src and @a dest have
 * to be aligned on 8 byte boundaries.  The size @a n must be evenly divisible by 8.
 * The SPE operations @b evxor, @b evlddx and @b evstddx will be used.
 */
GLOBAL_FUNCTION mpc55xx_copy_8
	/* Loop counter = data size / 8 */
	srwi. r5, r5, 3
	beqlr
	mtctr r5

	/* Set offset */
	evxor r5, r5, r5

copy_data:
	evlddx r6, r3, r5
	evstddx r6, r4, r5
	addi r5, r5, 8
	bdnz copy_data

	/* Return */
	blr
#endif /*!((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))*/
