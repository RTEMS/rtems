/**
 * @file
 *
 * @ingroup lpc32xx_boot
 *
 * @brief Boot support implementation.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <bsp/boot.h>

void lpc32xx_setup_boot_block(
  lpc32xx_boot_block *boot_block,
  uint8_t icr,
  uint8_t page_count
)
{
  memset(boot_block, 0, sizeof(*boot_block));

  ++page_count;

  boot_block->field.d0 = icr;
  boot_block->field.d2 = icr;
  boot_block->field.d4 = page_count;
  boot_block->field.d6 = page_count;
  boot_block->field.d8 = page_count;
  boot_block->field.d10 = page_count;

  icr = (uint8_t) ~((unsigned) icr);
  page_count = (uint8_t) ~((unsigned) page_count);

  boot_block->field.d1 = icr;
  boot_block->field.d3 = icr;
  boot_block->field.d5 = page_count;
  boot_block->field.d7 = page_count;
  boot_block->field.d9 = page_count;
  boot_block->field.d11 = page_count;

  boot_block->field.d12 = 0xaa;
}

void lpc32xx_set_boot_block_bad(
  lpc32xx_boot_block *boot_block
)
{
  boot_block->field.d12 = 0;
}
