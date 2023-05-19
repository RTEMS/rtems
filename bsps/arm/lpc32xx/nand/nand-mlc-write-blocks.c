/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_write_blocks() implementation.
 */

/*
 * Copyright (C) 2010, 2011 embedded brains GmbH & Co. KG
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

#include <bsp/nand-mlc.h>

#include <string.h>

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
