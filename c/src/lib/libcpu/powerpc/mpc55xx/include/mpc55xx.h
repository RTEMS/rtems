/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Documentation for this file
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

#include <mpc55xx/regs.h>
#include <mpc55xx/regs-mmu.h>

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int mpc55xx_flash_copy(void *dest, const void *src, size_t nbytes);
int mpc55xx_flash_copy_op(void *rdest, const void *src, size_t nbytes,
  uint32_t opmask, uint32_t *p_fail_addr);
int mpc55xx_flash_size(uint32_t *p_size);
int mpc55xx_flash_writable(void);
uint32_t mpc55xx_flash_address(void);
void mpc55xx_flash_set_read_only(void);
void mpc55xx_flash_set_read_write(void);

int mpc55xx_physical_address(const void *addr, uint32_t *p_result);
int mpc55xx_mapped_address(const void *addr, uint32_t *p_result);

/* Bits for opmask. */
#define MPC55XX_FLASH_BLANK_CHECK 0x01
#define MPC55XX_FLASH_UNLOCK      0x02
#define MPC55XX_FLASH_ERASE       0x04
#define MPC55XX_FLASH_PROGRAM     0x08
#define MPC55XX_FLASH_VERIFY      0x10

/* Error returns.  CONFIG or SIZE might mean you just
 * need to check for new configuration bits.
 * SIZE and RANGE mean you are outside of a known flash region.
 * ERASE means the erase failed,
 * PROGRAM means the program failed,
 * BLANK means it wasn't blank and BLANK_CHECK was specified,
 * VERIFY means VERIFY was set and it didn't match the source,
 * and LOCK means either the locking failed or you needed to
 * specify MPC55XX_FLASH_UNLOCK and didn't.
 */
#define MPC55XX_FLASH_CONFIG_ERR    (-1)
#define MPC55XX_FLASH_SIZE_ERR      (-2)
#define MPC55XX_FLASH_RANGE_ERR     (-3)
#define MPC55XX_FLASH_ERASE_ERR     (-4)
#define MPC55XX_FLASH_PROGRAM_ERR   (-5)
#define MPC55XX_FLASH_NOT_BLANK_ERR (-6)
#define MPC55XX_FLASH_VERIFY_ERR    (-7)
#define MPC55XX_FLASH_LOCK_ERR      (-8)

#define MPC55XX_CACHE_ALIGNED_MASK ((uintptr_t) 0x1f)

#define MPC55XX_CACHE_LINE_SIZE 32

/**
 * @brief Returns true if the buffer starting at @a s of size @a n is cache aligned.
 */
static inline int mpc55xx_is_cache_aligned( const void *s, size_t n)
{
	return !(((uintptr_t) s & MPC55XX_CACHE_ALIGNED_MASK) || (n & MPC55XX_CACHE_ALIGNED_MASK));
}

static inline uintptr_t mpc55xx_cache_aligned_start( const void *s)
{
	return ((uintptr_t) s & MPC55XX_CACHE_ALIGNED_MASK) ? (((uintptr_t) s & ~MPC55XX_CACHE_ALIGNED_MASK) + MPC55XX_CACHE_LINE_SIZE) : (uintptr_t)s;
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
	__asm__ (
		"cntlzw %0, %1;"
		: "=r" (count)
		: "r" (value)
	);
	return count;
}

static inline void mpc55xx_wait_for_interrupt(void)
{
  #ifdef MPC55XX_HAS_WAIT_INSTRUCTION
    __asm__ volatile ("wait");
  #else
    __asm__ volatile ("");
  #endif
}

static inline void mpc55xx_mmu_apply_config(const struct MMU_tag *config)
{
  PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS0, config->MAS0.R);
  PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1, config->MAS1.R);
  PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS2, config->MAS2.R);
  PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS3, config->MAS3.R);
  __asm__ volatile ("tlbwe");
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_H */
