/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  access.h  - access routines for SPARC. SPARC is big endian only.
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

#ifndef _LIBCPU_ACCESS_H
#define _LIBCPU_ACCESS_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* "Raw" access */
extern uint8_t _ld8(uint8_t *addr);
extern void _st8(uint8_t *addr, uint8_t val);
extern uint16_t _ld16(uint16_t *addr);
extern void _st16(uint16_t *addr, uint16_t val);
extern uint32_t _ld32(uint32_t *addr);
extern void _st32(uint32_t *addr, uint32_t val);
extern uint64_t _ld64(uint64_t *addr);
extern void _st64(uint64_t *addr, uint64_t val);

/* Aliases for Big Endian */
extern uint16_t _ld_be16(uint16_t *addr);
extern void _st_be16(uint16_t *addr, uint16_t val);
extern uint32_t _ld_be32(uint32_t *addr);
extern void _st_be32(uint32_t *addr, uint32_t val);
extern uint64_t _ld_be64(uint64_t *addr);
extern void _st_be64(uint64_t *addr, uint64_t val);

/* Little endian */
extern uint16_t _ld_le16(uint16_t *addr);
extern void _st_le16(uint16_t *addr, uint16_t val);
extern uint32_t _ld_le32(uint32_t *addr);
extern void _st_le32(uint32_t *addr, uint32_t val);

#ifdef __cplusplus
}
#endif

#endif
