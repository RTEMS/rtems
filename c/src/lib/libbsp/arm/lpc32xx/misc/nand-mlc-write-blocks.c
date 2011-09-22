/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_write_blocks() implementation.
 */

/*
 * Copyright (c) 2010-2011 embedded brains GmbH.  All rights reserved.
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

static const uint32_t ones_spare [MLC_LARGE_SPARE_WORD_COUNT] = {
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff
};

rtems_status_code lpc32xx_mlc_write_blocks(
  uint32_t block_begin,
  uint32_t block_end,
  const void *src,
  size_t src_size,
  uint32_t *page_data_buffer
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint32_t pages_per_block = lpc32xx_mlc_pages_per_block();
  uint32_t block_count = lpc32xx_mlc_block_count();
  uint32_t page_size = lpc32xx_mlc_page_size();
  uint32_t block = 0;
  const uint8_t *current = src;
  const uint8_t *last = current;
  const uint8_t *end = current + src_size;

  if (block_begin > block_end || block_end > block_count) {
    return RTEMS_INVALID_ID;
  }

  for (block = block_begin; block != block_end; ++block) {
    uint32_t page_begin = block * pages_per_block;
    uint32_t page_end = page_begin + pages_per_block;
    uint32_t page = 0;

    sc = lpc32xx_mlc_erase_block_safe_3(block, page_begin, page_end);
    if (sc != RTEMS_SUCCESSFUL) {
      continue;
    }

    for (page = page_begin; page < page_end; ++page) {
      uintptr_t remainder = (uintptr_t) end - (uintptr_t) current;
      size_t delta = remainder < page_size ? remainder : page_size;

      if (remainder > 0) {
        memcpy(page_data_buffer, current, delta);
        sc = lpc32xx_mlc_write_page_with_ecc(
          page,
          page_data_buffer,
          ones_spare
        );
        if (sc != RTEMS_SUCCESSFUL) {
          lpc32xx_mlc_erase_block(block);
          lpc32xx_mlc_zero_pages(page_begin, page_end);
          current = last;
          continue;
        }

        current += delta;
      } else {
        goto done;
      }
    }

    last = current;
  }

done:

  if (current != end) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}
