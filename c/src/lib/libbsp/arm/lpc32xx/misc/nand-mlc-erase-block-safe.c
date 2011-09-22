/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_erase_block_safe(), lpc32xx_mlc_erase_block_safe_3(), and
 * lpc32xx_mlc_zero_block() implementation.
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

void lpc32xx_mlc_zero_pages(uint32_t page_begin, uint32_t page_end)
{
  uint32_t page = 0;

  for (page = page_begin; page < page_end; ++page) {
    lpc32xx_mlc_write_page_with_ecc(
      page,
      lpc32xx_magic_zero_begin,
      lpc32xx_magic_zero_begin
    );
  }
}

static rtems_status_code is_valid_page(
  uint32_t page_begin,
  uint32_t page_offset
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint32_t spare [MLC_LARGE_SPARE_WORD_COUNT];

  memset(spare, 0, MLC_LARGE_SPARE_SIZE);

  sc = lpc32xx_mlc_read_page(
    page_begin + page_offset,
    lpc32xx_magic_zero_begin,
    spare,
    NULL
  );
  if (sc == RTEMS_SUCCESSFUL) {
    if (lpc32xx_mlc_is_bad_page(spare)) {
      sc = RTEMS_INCORRECT_STATE;
    }
  }

  return sc;
}

static rtems_status_code is_valid_block(uint32_t page_begin)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (lpc32xx_mlc_page_size() == 512 && lpc32xx_mlc_io_width() == 8) {
    sc = is_valid_page(page_begin, 0);
    if (sc == RTEMS_SUCCESSFUL) {
      sc = is_valid_page(page_begin, 1);
    }
  } else {
    sc = RTEMS_NOT_IMPLEMENTED;
  }

  return sc;
}

rtems_status_code lpc32xx_mlc_is_valid_block(uint32_t block_index)
{
  uint32_t pages_per_block = lpc32xx_mlc_pages_per_block();
  uint32_t page_begin = block_index * pages_per_block;

  return is_valid_block(page_begin);
}

rtems_status_code lpc32xx_mlc_erase_block_safe_3(
  uint32_t block_index,
  uint32_t page_begin,
  uint32_t page_end
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = is_valid_block(page_begin);
  if (sc == RTEMS_SUCCESSFUL) {
    sc = lpc32xx_mlc_erase_block(block_index);
    if (sc == RTEMS_UNSATISFIED) {
      lpc32xx_mlc_zero_pages(page_begin, page_end);
    }
  }

  return sc;
}

rtems_status_code lpc32xx_mlc_erase_block_safe(uint32_t block_index)
{
  uint32_t pages_per_block = lpc32xx_mlc_pages_per_block();
  uint32_t page_begin = block_index * pages_per_block;
  uint32_t page_end = page_begin + pages_per_block;

  return lpc32xx_mlc_erase_block_safe_3(
    block_index,
    page_begin,
    page_end
  );
}
