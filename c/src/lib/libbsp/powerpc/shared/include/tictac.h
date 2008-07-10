/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief Header file for tic-tac code.
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
 * @brief Reset reference ticks for tac().
 */
static inline void tic()
{
	uint32_t tmp;
	asm volatile (
		"mftb 0;"
		"stw 0, ppc_tic_tac@sdarel(13);"
		: "=r" (tmp)
	);
}

/**
 * @brief Returns number of ticks since last tic().
 */
static inline uint32_t tac()
{
	uint32_t ticks;
	uint32_t tmp;
	asm volatile (
		"mftb %0;"
		"lwz %1, ppc_tic_tac@sdarel(13);"
		"subf %0, %1, %0;"
		: "=r" (ticks), "=r" (tmp)
	);
	return ticks;
}

/**
 * @brief Reset reference ticks for bam().
 */
static inline void boom()
{
	uint32_t tmp;
	asm volatile (
		"mftb 0;"
		"stw 0, ppc_boom_bam@sdarel(13);"
		: "=r" (tmp)
	);
}

/**
 * @brief Returns number of ticks since last boom().
 */
static inline uint32_t bam()
{
	uint32_t ticks;
	uint32_t tmp;
	asm volatile (
		"mftb %0;"
		"lwz %1, ppc_boom_bam@sdarel(13);"
		"subf %0, %1, %0;"
		: "=r" (ticks), "=r" (tmp)
	);
	return ticks;
}
