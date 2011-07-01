/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_erase_block_safe() and lpc32xx_mlc_erase_block_safe_3()
 * implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <bsp/nand-mlc.h>

#include <string.h>

#include <bsp.h>

static void zero_block(uint32_t first_page_of_block, uint32_t pages_per_block)
{
  uint32_t page = 0;

  for (page = 0; page < pages_per_block; ++page) {
    lpc32xx_mlc_write_page_with_ecc(
      first_page_of_block + page,
      lpc32xx_magic_zero_begin,
      lpc32xx_magic_zero_begin
    );
  }
}

static bool is_bad_page(
  uint32_t first_page_of_block,
  uint32_t page
)
{
  uint32_t spare [MLC_LARGE_SPARE_WORD_COUNT];

  memset(spare, 0, MLC_LARGE_SPARE_SIZE);
  lpc32xx_mlc_read_page(
    first_page_of_block + page,
    lpc32xx_magic_zero_begin,
    spare
  );
  return lpc32xx_mlc_is_bad_page(spare);
}

rtems_status_code lpc32xx_mlc_erase_block_safe_3(
  uint32_t block_index,
  uint32_t first_page_of_block,
  uint32_t pages_per_block
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (is_bad_page(first_page_of_block, 0)) {
    return RTEMS_INCORRECT_STATE;
  }

  if (is_bad_page(first_page_of_block, 1)) {
    return RTEMS_INCORRECT_STATE;
  }

  sc = lpc32xx_mlc_erase_block(block_index);
  if (sc != RTEMS_SUCCESSFUL) {
    zero_block(first_page_of_block, pages_per_block);

    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc32xx_mlc_erase_block_safe(uint32_t block_index)
{
  uint32_t pages_per_block = lpc32xx_mlc_pages_per_block();

  return lpc32xx_mlc_erase_block_safe_3(
    block_index,
    block_index * pages_per_block,
    pages_per_block
  );
}
