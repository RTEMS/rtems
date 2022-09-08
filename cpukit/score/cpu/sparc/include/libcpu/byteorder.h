/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  byteorder.h  - Endian conversion for SPARC. SPARC is big endian only.
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler.
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

#ifndef _LIBCPU_BYTEORDER_H
#define _LIBCPU_BYTEORDER_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint16_t ld_le16(volatile uint16_t *addr)
{
	return CPU_swap_u16(*addr);
}

static inline void st_le16(volatile uint16_t *addr, uint16_t val)
{
	*addr = CPU_swap_u16(val);
}

static inline uint32_t ld_le32(volatile uint32_t *addr)
{
	return CPU_swap_u32(*addr);
}

static inline void st_le32(volatile uint32_t *addr, uint32_t val)
{
	*addr = CPU_swap_u32(val);
}

static inline uint16_t ld_be16(volatile uint16_t *addr)
{
	return *addr;
}

static inline void st_be16(volatile uint16_t *addr, uint16_t val)
{
	*addr = val;
}

static inline uint32_t ld_be32(volatile uint32_t *addr)
{
	return *addr;
}

static inline void st_be32(volatile uint32_t *addr, uint32_t val)
{
	*addr = val;
}

#ifdef __cplusplus
}
#endif

#endif
