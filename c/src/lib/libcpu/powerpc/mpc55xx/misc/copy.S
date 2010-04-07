/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Memory copy and zero functions.
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

.section ".text"

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

/**
 * @fn int mpc55xx_zero_4( void *dest, size_t n)
 *
 * @brief Zero all @a n bytes starting at @a dest with 4 byte writes.
 *
 * The address @a dest has to be aligned on 4 byte boundaries.  The size @a n
 * must be evenly divisible by 4.  No SPE operations are used.
 */
#if       ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))
GLOBAL_FUNCTION mpc55xx_zero_32
GLOBAL_FUNCTION mpc55xx_zero_8
#endif /* ((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517)) */
GLOBAL_FUNCTION mpc55xx_zero_4
	/* Create zero */
	xor r0, r0, r0

	/* Loop counter for the first bytes up to 16 bytes */
	rlwinm. r9, r4, 29, 30, 31
	beq zero_more4
	mtctr r9
	xor   r5,r5,r5

zero_data4:
	stwx r0, r5, r3
	addi r5, r5, 4
	bdnz zero_data4

zero_more4:
	/* More than 16 bytes? */
	srwi. r9, r4, 4
	beqlr
	mtctr r9

zero_big_data4:
	stw r0,  0(r3)
	stw r0,  4(r3)
	stw r0,  8(r3)
	stw r0, 12(r3)
	addi r3, r3, 16
	bdnz zero_big_data4
	/* Return */
	blr
#if      !((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517))
/**
 * @fn int mpc55xx_zero_8( void *dest, size_t n)
 *
 * @brief Zero all @a n bytes starting at @a dest with 8 byte writes.
 *
 * The address @a dest has to be aligned on 8 byte boundaries.  The size @a n
 * must be evenly divisible by 8.  The SPE operations @b evxor and @b evstddx will be used.
 */
GLOBAL_FUNCTION mpc55xx_zero_8
	/* Create zero */
	evxor r0, r0, r0

	/* Set offset */
	evxor r5, r5, r5

	/* Loop counter for the first bytes up to 32 bytes */
	rlwinm. r9, r4, 29, 30, 31
	beq zero_more
	mtctr r9

zero_data:
	evstddx r0, r3, r5
	addi r5, r5, 8
	bdnz zero_data

zero_more:
	/* More than 32 bytes? */
	srwi. r9, r4, 5
	beqlr
	mtctr r9

	/* Set offsets */
	addi r6, r5, 8
	addi r7, r5, 16
	addi r8, r5, 24

zero_big_data:
	evstddx r0, r3, r5
	addi r5, r5, 32
	evstddx r0, r3, r6
	addi r6, r6, 32
	evstddx r0, r3, r7
	addi r7, r7, 32
	evstddx r0, r3, r8
	addi r8, r8, 32
	bdnz zero_big_data
	/* Return */
	blr

/**
 * @fn int mpc55xx_zero_32( void *dest, size_t n)
 *
 * @brief Zero all @a n bytes starting at @a dest with 32 byte writes.
 *
 * The address @a dest has to be aligned on 32 byte boundaries.  The size @a n
 * must be evenly divisible by 32.  The function operates with the cache block zero
 * operation @b dcbz.
 *
 * @note The cache has to be enabled for the desired memory area.
 */
GLOBAL_FUNCTION mpc55xx_zero_32
	/* Set offset */
	xor r5, r5, r5

	/* Loop counter for the first bytes up to 128 bytes */
	rlwinm. r9, r4, 27, 28, 31
	beq zero_more_lines
	mtctr r9

zero_line:
	dcbz r3, r5
	addi r5, r5, 32
	bdnz zero_line

zero_more_lines:
	/* More than 128 bytes? */
	srwi. r9, r4, 7
	beqlr
	mtctr r9

	/* Set offsets */
	addi r6, r5, 32
	addi r7, r5, 64
	addi r8, r5, 96

zero_big_line:
	dcbz r3, r5
	addi r5, r5, 128
	dcbz r3, r6
	addi r6, r6, 128
	dcbz r3, r7
	addi r7, r7, 128
	dcbz r3, r8
	addi r8, r8, 128
	bdnz zero_big_line

	/* Return */
	blr
#endif  /*  !((MPC55XX_CHIP_TYPE>=5510) && (MPC55XX_CHIP_TYPE<=5517)) */
