/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <sys/param.h>

#include <libfdt.h>

#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>

#ifndef BSP_FDT_IS_SUPPORTED
#warning "BSP FDT support indication not defined"
#endif

#ifndef BSP_FDT_BLOB_SIZE_MAX
#define BSP_FDT_BLOB_SIZE_MAX 0
#endif

#ifdef BSP_FDT_BLOB_READ_ONLY
static const uint32_t
bsp_fdt_blob[BSP_FDT_BLOB_SIZE_MAX / sizeof(uint32_t)] = { 0xdeadbeef };
#else
static uint32_t
bsp_fdt_blob[BSP_FDT_BLOB_SIZE_MAX / sizeof(uint32_t)];
#endif

void bsp_fdt_copy(const void *src)
{
  const uint32_t *s = (const uint32_t *) src;
#ifdef BSP_FDT_BLOB_READ_ONLY
  uint32_t *d = (uint32_t *) ((uintptr_t) &bsp_fdt_blob[0]
    - (uintptr_t) bsp_section_rodata_begin
    + (uintptr_t) bsp_section_rodata_load_begin);
#else
  uint32_t *d = &bsp_fdt_blob[0];
#endif

  if (s != d) {
    uint32_t m = MIN(sizeof(bsp_fdt_blob), fdt_totalsize(src));
    uint32_t n = (m + sizeof(*d) - 1) / sizeof(*d);
    uint32_t i;

    for (i = 0; i < n; ++i) {
      d[i] = s[i];
    }

    rtems_cache_flush_multiple_data_lines(d, m);
  }
}

const void *bsp_fdt_get(void)
{
  return &bsp_fdt_blob[0];
}
