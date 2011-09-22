/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief lpc32xx_mlc_read_blocks() implementation.
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

static rtems_status_code read_page(
  uint32_t first_page_of_block,
  uint32_t page,
  uint32_t page_data [MLC_LARGE_DATA_WORD_COUNT],
  uint32_t page_spare [MLC_LARGE_SPARE_WORD_COUNT]
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint32_t page_index = first_page_of_block + page;
  bool possible_bad_page = page == 0 || page == 1;

  if (possible_bad_page) {
    memset(page_spare, 0, MLC_LARGE_SPARE_SIZE);
  }

  sc = lpc32xx_mlc_read_page(page_index, page_data, page_spare, NULL);
  if (possible_bad_page && lpc32xx_mlc_is_bad_page(page_spare)) {
    return RTEMS_UNSATISFIED;
  } else if (sc == RTEMS_SUCCESSFUL) {
    return RTEMS_SUCCESSFUL;
  } else {
    return sc;
  }
}

rtems_status_code lpc32xx_mlc_read_blocks(
  uint32_t block_begin,
  uint32_t block_end,
  lpc32xx_mlc_read_process process,
  void *process_arg,
  uint32_t page_buffer_0 [MLC_LARGE_DATA_WORD_COUNT],
  uint32_t page_buffer_1 [MLC_LARGE_DATA_WORD_COUNT]
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint32_t page_spare_0 [MLC_LARGE_SPARE_WORD_COUNT];
  uint32_t page_spare_1 [MLC_LARGE_SPARE_WORD_COUNT];
  uint32_t pages_per_block = lpc32xx_mlc_pages_per_block();
  uint32_t page_size = lpc32xx_mlc_page_size();
  uint32_t block = 0;
  uint32_t first_page_of_block = block_begin * pages_per_block;

  for (
    block = block_begin;
    block != block_end;
    ++block, first_page_of_block += pages_per_block
  ) {
    uint32_t page = 0;
    bool done = false;

    sc = read_page(first_page_of_block, 0, page_buffer_0, page_spare_0);
    if (sc == RTEMS_UNSATISFIED) {
      continue;
    } else if (sc != RTEMS_SUCCESSFUL) {
      goto done;
    }

    sc = read_page(first_page_of_block, 1, page_buffer_1, page_spare_1);
    if (sc == RTEMS_UNSATISFIED) {
      continue;
    } else if (sc != RTEMS_SUCCESSFUL) {
      goto done;
    }

    done = (*process)(
      process_arg,
      first_page_of_block + 0,
      page_size,
      page_buffer_0,
      page_spare_0
    );
    if (done) {
      goto done;
    }

    done = (*process)(
      process_arg,
      first_page_of_block + 1,
      page_size,
      page_buffer_1,
      page_spare_1
    );
    if (done) {
      goto done;
    }

    for (page = 2; page < pages_per_block; ++page) {
      sc = read_page(first_page_of_block, page, page_buffer_1, page_spare_1);
      if (sc != RTEMS_SUCCESSFUL) {
        goto done;
      }

      done = (*process)(
        process_arg,
        first_page_of_block + page,
        page_size,
        page_buffer_1,
        page_spare_1
      );
      if (done) {
        goto done;
      }
    }
  }

done:

  return sc;
}
