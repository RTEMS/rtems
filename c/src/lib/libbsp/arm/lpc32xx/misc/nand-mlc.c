/**
 * @file
 *
 * @ingroup lpc32xx_nand_mlc
 *
 * @brief NAND MLC controller implementation.
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

#include <bsp/lpc32xx.h>
#include <bsp/nand-mlc.h>

static volatile lpc32xx_nand_mlc *const mlc = &lpc32xx.nand_mlc;

static uint32_t mlc_flags;

static uint32_t mlc_block_count;

static uint32_t mlc_page_count;

static bool mlc_small_pages(void)
{
  return (mlc_flags & MLC_SMALL_PAGES) != 0;
}

static bool mlc_many_address_cycles(void)
{
  return (mlc_flags & MLC_MANY_ADDRESS_CYCLES) != 0;
}

static bool mlc_normal_blocks(void)
{
  return (mlc_flags & MLC_NORMAL_BLOCKS) != 0;
}

uint32_t lpc32xx_mlc_page_size(void)
{
  if (mlc_small_pages()) {
    return 512;
  } else {
    return 2048;
  }
}

uint32_t lpc32xx_mlc_pages_per_block(void)
{
  if (mlc_small_pages()) {
    return 32;
  } else {
    if (mlc_normal_blocks()) {
      return 64;
    } else {
      return 128;
    }
  }
}

uint32_t lpc32xx_mlc_block_count(void)
{
  return mlc_block_count;
}

uint32_t lpc32xx_mlc_io_width(void)
{
  if ((mlc_flags & MLC_IO_WIDTH_16_BIT) == 0) {
    return 8;
  } else {
    return 16;
  }
}

static void mlc_unlock(void)
{
  mlc->lock_pr = MLC_UNLOCK_PROT;
}

static void mlc_wait(uint32_t flags)
{
  while ((mlc->isr & flags) != flags) {
    /* Wait */
  }
}

static void mlc_wait_until_ready(void)
{
  mlc_wait(MLC_ISR_CONTROLLER_READY | MLC_ISR_NAND_READY);
}

static void mlc_reset(void)
{
  mlc->cmd = 0xff;
}

static uint32_t mlc_status(void)
{
  mlc_wait_until_ready();
  mlc->cmd = 0x70;

  return mlc->data.w8;
}

static bool mlc_was_operation_successful(void)
{
  return (mlc_status() & (NAND_STATUS_READY | NAND_STATUS_ERROR))
    == NAND_STATUS_READY;
}

static void mlc_set_block_address(uint32_t block_index)
{
  if (mlc_small_pages()) {
    mlc->addr = (uint8_t) (block_index << 5);
    mlc->addr = (uint8_t) (block_index >> 3);
    if (mlc_many_address_cycles()) {
      mlc->addr = (uint8_t) (block_index >> 11);
    }
  } else {
    if (mlc_normal_blocks()) {
      mlc->addr = (uint8_t) (block_index << 6);
      mlc->addr = (uint8_t) (block_index >> 2);
      if (mlc_many_address_cycles()) {
        mlc->addr = (uint8_t) (block_index >> 10);
      }
    } else {
      mlc->addr = (uint8_t) (block_index << 7);
      mlc->addr = (uint8_t) (block_index >> 1);
      if (mlc_many_address_cycles()) {
        mlc->addr = (uint8_t) (block_index >> 9);
      }
    }
  }
}

static void mlc_set_page_address(uint32_t page_index)
{
  mlc->addr = 0;
  if (mlc_small_pages()) {
    mlc->addr = (uint8_t) page_index;
    mlc->addr = (uint8_t) (page_index >> 8);
    if (mlc_many_address_cycles()) {
      mlc->addr = (uint8_t) (page_index >> 16);
    }
  } else {
    mlc->addr = 0;
    mlc->addr = (uint8_t) page_index;
    mlc->addr = (uint8_t) (page_index >> 8);
    if (mlc_many_address_cycles()) {
      mlc->addr = (uint8_t) (page_index >> 16);
    }
  }
}

void lpc32xx_mlc_init(const lpc32xx_mlc_config *cfg)
{
  uint32_t icr = 0;

  mlc_flags = cfg->flags;
  mlc_block_count = cfg->block_count;
  mlc_page_count = cfg->block_count * lpc32xx_mlc_pages_per_block();

  /* Clock */
  LPC32XX_FLASHCLK_CTRL = FLASHCLK_IRQ_MLC | FLASHCLK_MLC_CLK_ENABLE;

  /* Timing settings */
  mlc_unlock();
  mlc->time = cfg->time;

  /* Configuration */
  if (!mlc_small_pages()) {
    icr |= MLC_ICR_LARGE_PAGES;
  }
  if (mlc_many_address_cycles()) {
    icr |= MLC_ICR_ADDR_WORD_COUNT_4_5;
  }
  mlc_unlock();
  mlc->icr = icr;

  mlc_reset();
}

void lpc32xx_mlc_write_protection(
  uint32_t page_index_low,
  uint32_t page_index_high
)
{
  mlc_unlock();
  mlc->sw_wp_add_low = page_index_low;
  mlc_unlock();
  mlc->sw_wp_add_hig = page_index_high;
  mlc_unlock();
  mlc->icr |= MLC_ICR_SOFT_WRITE_PROT;
}

bool is_word_aligned(const void *data, const void *spare)
{
  return (((uintptr_t) data) | ((uintptr_t) spare)) % 4 == 0;
}

rtems_status_code lpc32xx_mlc_read_page(
  uint32_t page_index,
  void *data,
  void *spare,
  uint32_t *symbol_error_count_ptr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t small_pages_count = mlc_small_pages() ? 1 : MLC_SMALL_PAGES_PER_LARGE_PAGE;
  size_t sp = 0;
  size_t i = 0;
  uint32_t isr = 0;
  uint32_t symbol_error_count = 0xffffffff;
  bool aligned = is_word_aligned(data, spare);
  uint8_t *current_data = data;
  uint8_t *current_spare = spare;

  if (page_index >= mlc_page_count) {
    return RTEMS_INVALID_ID;
  }

  mlc_wait_until_ready();
  mlc->cmd = 0x00;
  if (!mlc_small_pages()) {
    mlc->cmd = 0x30;
  }
  mlc_set_page_address(page_index);
  mlc_wait(MLC_ISR_NAND_READY);

  for (sp = 0; sc == RTEMS_SUCCESSFUL && sp < small_pages_count; ++sp) {
    uint32_t *aligned_data = (uint32_t *) current_data;
    uint32_t *aligned_spare = (uint32_t *) current_spare;

    mlc->ecc_dec = 0;

    if (aligned) {
      for (i = 0; i < MLC_SMALL_DATA_WORD_COUNT; ++i) {
        aligned_data [i] = mlc->data.w32;
      }
      for (i = 0; i < MLC_SMALL_SPARE_WORD_COUNT; ++i) {
        aligned_spare [i] = mlc->data.w32;
      }
    } else {
      for (i = 0; i < MLC_SMALL_DATA_SIZE; ++i) {
        current_data [i] = mlc->data.w8;
      }
      for (i = 0; i < MLC_SMALL_SPARE_SIZE; ++i) {
        current_spare [i] = mlc->data.w8;
      }
    }

    mlc_wait(MLC_ISR_ECC_READY);

    isr = mlc->isr;
    if ((isr & MLC_ISR_ERRORS_DETECTED) == 0) {
      symbol_error_count = 0;
    } else {
      if ((isr & MLC_ISR_DECODER_FAILURE) == 0) {
        symbol_error_count = MLC_ISR_SYMBOL_ERRORS(isr);
        if (aligned) {
          mlc->rubp = 0;
          for (i = 0; i < MLC_SMALL_DATA_WORD_COUNT; ++i) {
            aligned_data [i] = mlc->buff.w32;
          }
          mlc->robp = 0;
          for (i = 0; i < MLC_SMALL_SPARE_WORD_COUNT; ++i) {
            aligned_spare [i] = mlc->buff.w32;
          }
        } else {
          mlc->rubp = 0;
          for (i = 0; i < MLC_SMALL_DATA_SIZE; ++i) {
            current_data [i] = mlc->buff.w8;
          }
          mlc->robp = 0;
          for (i = 0; i < MLC_SMALL_SPARE_SIZE; ++i) {
            current_spare [i] = mlc->buff.w8;
          }
        }
      } else {
        sc = RTEMS_IO_ERROR;
      }
    }

    current_data += MLC_SMALL_DATA_SIZE;
    current_spare += MLC_SMALL_SPARE_SIZE;
  }

  if (symbol_error_count_ptr != NULL) {
    *symbol_error_count_ptr = symbol_error_count;
  }

  return sc;
}

void lpc32xx_mlc_read_id(uint8_t *id, size_t n)
{
  size_t i = 0;

  mlc_wait_until_ready();
  mlc->cmd = 0x90;
  mlc->addr = 0;
  mlc_wait(MLC_ISR_NAND_READY);

  for (i = 0; i < n; ++i) {
    id [i] = mlc->data.w8;
  }
}

rtems_status_code lpc32xx_mlc_erase_block(uint32_t block_index)
{
  rtems_status_code sc = RTEMS_UNSATISFIED;

  if (block_index >= mlc_block_count) {
    return RTEMS_INVALID_ID;
  }

  mlc_wait_until_ready();
  mlc->cmd = 0x60;
  mlc_set_block_address(block_index);
  mlc->cmd = 0xd0;

  if (mlc_was_operation_successful()) {
    sc = RTEMS_SUCCESSFUL;
  }

  return sc;
}

rtems_status_code lpc32xx_mlc_write_page_with_ecc(
  uint32_t page_index,
  const void *data,
  const void *spare
)
{
  rtems_status_code sc = RTEMS_IO_ERROR;
  size_t small_pages_count = mlc_small_pages() ?
    1 : MLC_SMALL_PAGES_PER_LARGE_PAGE;
  size_t sp = 0;
  size_t i = 0;
  bool aligned = is_word_aligned(data, spare);
  const uint8_t *current_data = data;
  const uint8_t *current_spare = spare;

  if (page_index >= mlc_page_count) {
    return RTEMS_INVALID_ID;
  }

  mlc_wait_until_ready();
  mlc->cmd = 0x80;
  mlc_set_page_address(page_index);

  for (sp = 0; sp < small_pages_count; ++sp) {
    mlc->ecc_enc = 0;

    if (aligned) {
      const uint32_t *aligned_data = (const uint32_t *) current_data;
      const uint32_t *aligned_spare = (const uint32_t *) current_spare;

      for (i = 0; i < MLC_SMALL_DATA_WORD_COUNT; ++i) {
        mlc->data.w32 = aligned_data [i];
      }
      mlc->data.w32 = aligned_spare [0];
      mlc->data.w16 = (uint16_t) aligned_spare [1];
    } else {
      for (i = 0; i < MLC_SMALL_DATA_SIZE; ++i) {
        mlc->data.w8 = current_data [i];
      }
      for (i = 0; i < MLC_SMALL_USER_SPARE_SIZE; ++i) {
        mlc->data.w8 = current_spare [i];
      }
    }
    mlc->wpr = 0;

    mlc_wait(MLC_ISR_CONTROLLER_READY);

    current_data += MLC_SMALL_DATA_SIZE;
    current_spare += MLC_SMALL_SPARE_SIZE;
  }

  mlc->cmd = 0x10;

  if (mlc_was_operation_successful()) {
    sc = RTEMS_SUCCESSFUL;
  }

  return sc;
}
