/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Kevin Kirspel
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

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003, 2007 Altera Corporation, San Jose, California, USA.     *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#ifndef __ALT_CACHE_H__
#define __ALT_CACHE_H__

/*
 * Cache maintenance macros
 * CLEAN - Writeback to memory;
 * FLUSH - Writeback to memory and invalidate.
 */

#if ALT_CPU_DCACHE_SIZE > 0
#define DCACHE_CLEAN_BY_INDEX_VAL(i) \
__asm__ volatile(".insn i 0x0F, 0x2, zero, %[i_reg], 0x081" :: [i_reg] "r"(i));

#define DCACHE_FLUSH_BY_INDEX_VAL(i) \
__asm__ volatile(".insn i 0x0F, 0x2, zero, %[i_reg], 0x082" :: [i_reg] "r"(i));

#define DCACHE_INVALIDATE_BY_INDEX_VAL(i) \
__asm__ volatile(".insn i 0x0F, 0x2, zero, %[i_reg], 0x080" :: [i_reg] "r"(i));

#define ALT_FLUSH_DATA(i) \
__asm__ volatile(".option arch, +zicbom\n" "cbo.flush 0(%[addr])" :: [addr] "r"(i))

#define ALT_INVALIDATE_DATA(i) \
__asm__ volatile(".option arch, +zicbom\n" "cbo.inval 0(%[addr])" :: [addr] "r"(i))
#endif

/*
 * alt_cache.h defines the processor specific functions for manipulating the
 * cache.
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * alt_icache_flush() is called to flush the instruction cache for a memory
 * region of length "len" bytes, starting at address "start".
 */

static inline void alt_icache_flush (const void* start, uint32_t len)
{
#if ALT_CPU_ICACHE_SIZE > 0
  __asm__ volatile(".option arch, +zifencei\n" "fence.i" ::: "memory");
#else
  (void) start;
  (void) len;
#endif
}

/*
 * alt_dcache_flush() is called to flush the data cache for a memory
 * region of length "len" bytes, starting at address "start".
 * Any dirty lines in the data cache are written back to memory.
 */

static inline void alt_dcache_flush (const void* start, uint32_t len)
{
#if ALT_CPU_DCACHE_SIZE > 0
  const char* i;
  const char* end = ((char*)start) + len;

  for (i = start; i < end; i+= ALT_CPU_DCACHE_LINE_SIZE) {
    ALT_FLUSH_DATA(i);
  }

  /*
  * For an unaligned flush request, we've got one more line left.
  * Note that this is dependent on ALT_CPU_DCACHE_LINE_SIZE to be a
  * multiple of 2 (which it always is).
  */
  if (((uint32_t)start) & (ALT_CPU_DCACHE_LINE_SIZE - 1)) {
    ALT_FLUSH_DATA(i);
  }
#else
  (void) start;
  (void) len;
#endif
}

/*
 * alt_dcache_flush() is called to flush the data cache for a memory
 * region of length "len" bytes, starting at address "start".
 * Any dirty lines in the data cache are NOT written back to memory.
 */

static inline void alt_dcache_flush_no_writeback (
  const void* start,
  uint32_t len
)
{
#if ALT_CPU_DCACHE_SIZE > 0
  const char* i;
  const char* end = ((char*)start) + len;

  for (i = start; i < end; i+= ALT_CPU_DCACHE_LINE_SIZE) {
    ALT_INVALIDATE_DATA(i);
  }

  /*
  * For an unaligned invalidate request, we've got one more line left.
  * Note that this is dependent on ALT_CPU_DCACHE_LINE_SIZE to be a
  * multiple of 2 (which it always is).
  */
  if (((uint32_t)start) & (ALT_CPU_DCACHE_LINE_SIZE - 1)) {
    ALT_INVALIDATE_DATA(i);
  }
#else
  (void) start;
  (void) len;
#endif
}

/*
 * Flush the entire instruction cache.
 */

static inline void alt_icache_flush_all (void)
{
#if ALT_CPU_ICACHE_SIZE > 0
  __asm__ volatile(".option arch, +zifencei\n" "fence.i" ::: "memory");
#endif
}

/*
 * Flush the entire data cache.
 */

static inline void alt_dcache_flush_all (void)
{
#if ALT_CPU_DCACHE_SIZE > 0
  char* i;
  for (
    i = (char*)0;
    i < (char*) ALT_CPU_DCACHE_SIZE;
    i+= ALT_CPU_DCACHE_LINE_SIZE
  ) {
    DCACHE_CLEAN_BY_INDEX_VAL(i);
  }
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __ALT_CACHE_H__ */
