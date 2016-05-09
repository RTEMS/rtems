/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <bsp/qoriq.h>

#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)

#include <libcpu/powerpc-utility.h>

void qoriq_clear_ce_portal(void *base, size_t size)
{
  size_t offset;

  for (offset = 0; offset < size; offset += 64) {
    ppc_data_cache_block_clear_to_zero_2(base, offset);
    ppc_data_cache_block_flush_2(base, offset);
  }
}

void qoriq_clear_ci_portal(void *base, size_t size)
{
  uint32_t zero;
  size_t offset;

  zero = 0;

  for (offset = 0; offset < size; offset += 4) {
    ppc_write_word(zero, (char *) base + offset);
  }
}

#endif
