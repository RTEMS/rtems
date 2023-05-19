/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCShared
 *
 * @brief Header file for tic-tac code.
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

/**
 * @brief Reset reference ticks for tac().
 */
static inline void tic(void)
{
	uint32_t tmp;
	__asm__ volatile (
		"mftb 0;"
		"stw 0, ppc_tic_tac@sdarel(13);"
		: "=r" (tmp)
	);
}

/**
 * @brief Returns number of ticks since last tic().
 */
static inline uint32_t tac(void)
{
	uint32_t ticks;
	uint32_t tmp;
	__asm__ volatile (
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
static inline void boom(void)
{
	uint32_t tmp;
	__asm__ volatile (
		"mftb 0;"
		"stw 0, ppc_boom_bam@sdarel(13);"
		: "=r" (tmp)
	);
}

/**
 * @brief Returns number of ticks since last boom().
 */
static inline uint32_t bam(void)
{
	uint32_t ticks;
	uint32_t tmp;
	__asm__ volatile (
		"mftb %0;"
		"lwz %1, ppc_boom_bam@sdarel(13);"
		"subf %0, %1, %0;"
		: "=r" (ticks), "=r" (tmp)
	);
	return ticks;
}
