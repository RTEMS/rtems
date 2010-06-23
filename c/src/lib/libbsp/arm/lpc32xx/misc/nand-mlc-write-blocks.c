/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_write_blocks() implementation.
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

#include <bsp/nand-mlc.h>

#include <string.h>

#include <bsp.h>

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

static rtems_status_code erase_block(
  uint32_t block,
  uint32_t first_page_of_block,
  uint32_t pages_per_block
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (is_bad_page(first_page_of_block, 0)) {
    return RTEMS_IO_ERROR;
  }

  if (is_bad_page(first_page_of_block, 1)) {
    return RTEMS_IO_ERROR;
  }

  sc = lpc32xx_mlc_erase_block(block);
  if (sc != RTEMS_SUCCESSFUL) {
    zero_block(first_page_of_block, pages_per_block);

    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

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
    uint32_t first_page_of_block = block * pages_per_block;
    uint32_t page = 0;

    sc = erase_block(block, first_page_of_block, pages_per_block);
    if (sc != RTEMS_SUCCESSFUL) {
      continue;
    }

    for (page = 0; page < pages_per_block; ++page) {
      uintptr_t remainder = (uintptr_t) end - (uintptr_t) current;
      size_t delta = remainder < page_size ? remainder : page_size;

      if (remainder > 0) {
        memcpy(page_data_buffer, current, delta);
        sc = lpc32xx_mlc_write_page_with_ecc(
          first_page_of_block + page,
          page_data_buffer,
          ones_spare
        );
        if (sc != RTEMS_SUCCESSFUL) {
          erase_block(block, first_page_of_block, pages_per_block);
          zero_block(first_page_of_block, pages_per_block);
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
