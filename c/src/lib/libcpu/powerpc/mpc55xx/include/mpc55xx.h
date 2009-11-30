/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Documentation for this file
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

/**
 * @defgroup mpc55xx BSP for MPC55xx boards
 */

/**
 * @defgroup mpc55xx_config Configuration files
 *
 * @ingroup mpc55xx
 *
 * Makefiles, configure scripts etc.
 */

/**
 * @page mpc55xx_ext_doc External Documentation
 *
 * @section mpc55xx_ext_doc_mpc5567rm_1 MPC5567 Microcontroller Reference Manual (Rev. 1, January 2007, Volume 1 of 2)
 * @section mpc55xx_ext_doc_mpc5567rm_2 MPC5567 Microcontroller Reference Manual (Rev. 1, January 2007, Volume 2 of 2)
 */

#ifndef LIBCPU_POWERPC_MPC55XX_H
#define LIBCPU_POWERPC_MPC55XX_H

#include <stddef.h>
#include <stdint.h>

/* Defined in copy.S */
int mpc55xx_copy_8( const void *src, void *dest, size_t n);

/* Defined in copy.S */
int mpc55xx_zero_8( void *dest, size_t n);

/* Defined in copy.S */
int mpc55xx_zero_32( void *dest, size_t n);

/* Defined in fmpll.S */
void mpc55xx_fmpll_reset_config();

/* Defined in fmpll.S */
void mpc55xx_fmpll_wait_for_lock();

/* Defined in fmpll.S */
int mpc55xx_get_system_clock();

/* Defined in fmpll.S */
void mpc55xx_system_reset();

/* Defined in flash.S */
void mpc55xx_flash_config();

#define MPC55XX_CACHE_ALIGNED_MASK ((uintptr_t) 0x1f)

#define MPC55XX_CACHE_LINE_SIZE 32

/**
 * @brief Returns true if the buffer starting at @a s of size @a n is cache aligned.
 */
static inline int mpc55xx_is_cache_aligned( const void *s, size_t n)
{
	return !(((uintptr_t) s & MPC55XX_CACHE_ALIGNED_MASK) || (n & MPC55XX_CACHE_ALIGNED_MASK));
}

static inline void* mpc55xx_cache_aligned_start( const void *s)
{
	return ((uintptr_t) s & MPC55XX_CACHE_ALIGNED_MASK) ? (((uintptr_t) s & ~MPC55XX_CACHE_ALIGNED_MASK) + MPC55XX_CACHE_LINE_SIZE) : s;
}

static inline size_t mpc55xx_non_cache_aligned_size( const void *s)
{
	return (uintptr_t) mpc55xx_cache_aligned_start( s) - (uintptr_t) s;
}

static inline size_t mpc55xx_cache_aligned_size( const void *s, size_t n)
{
	return (n - mpc55xx_non_cache_aligned_size( s)) & ~MPC55XX_CACHE_ALIGNED_MASK;
}

/**
 * @brief Returns the number of leading zeros.
 */
static inline uint32_t mpc55xx_count_leading_zeros( uint32_t value)
{
	uint32_t count;
	asm (
		"cntlzw %0, %1;"
		: "=r" (count)
		: "r" (value)
	);
	return count;
}

#endif /* LIBCPU_POWERPC_MPC55XX_H */
