/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Memory copy functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
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
