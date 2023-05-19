/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_erase_block_safe(), lpc32xx_mlc_erase_block_safe_3(), and
 * lpc32xx_mlc_zero_block() implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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
