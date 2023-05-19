/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2017 embedded brains GmbH & Co. KG
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

#include <sys/param.h>

#include <libfdt.h>

#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>

#ifdef BSP_DTB_IS_SUPPORTED
#include BSP_DTB_HEADER_PATH
#endif

#ifndef BSP_FDT_IS_SUPPORTED
#warning "BSP FDT support indication not defined"
#endif

#ifndef BSP_FDT_BLOB_SIZE_MAX
#define BSP_FDT_BLOB_SIZE_MAX 0
#endif

#ifdef BSP_FDT_BLOB_READ_ONLY
static RTEMS_ALIGNED(8) const uint32_t
bsp_fdt_blob[BSP_FDT_BLOB_SIZE_MAX / sizeof(uint32_t)] =
  { 0xdeadbeef };
#else
static RTEMS_ALIGNED(8) uint32_t
bsp_fdt_blob[BSP_FDT_BLOB_SIZE_MAX / sizeof(uint32_t)];
#endif

void bsp_fdt_copy(const void *src)
{
  const volatile uint32_t *s = (const uint32_t *) src;
#ifdef BSP_FDT_BLOB_COPY_TO_READ_ONLY_LOAD_AREA
  uint32_t *d = (uint32_t *) ((uintptr_t) &bsp_fdt_blob[0]
    - (uintptr_t) bsp_section_rodata_begin
    + (uintptr_t) bsp_section_rodata_load_begin);
#else
  uint32_t *d = RTEMS_DECONST(uint32_t *, &bsp_fdt_blob[0]);
#endif

  if (s != d) {
    size_t m = MIN(sizeof(bsp_fdt_blob), fdt_totalsize(src));
    size_t aligned_size = roundup2(m, CPU_CACHE_LINE_BYTES);
    size_t n = (m + sizeof(*d) - 1) / sizeof(*d);
    size_t i;

    for (i = 0; i < n; ++i) {
      d[i] = s[i];
    }

    rtems_cache_flush_multiple_data_lines(d, aligned_size);
  }
}

const void *bsp_fdt_get(void)
{
#ifdef BSP_DTB_IS_SUPPORTED
  return system_dtb;
#else
  return &bsp_fdt_blob[0];
#endif
}
