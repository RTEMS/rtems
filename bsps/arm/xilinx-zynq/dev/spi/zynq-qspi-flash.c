/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Contemporary Software
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#include <rtems.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include <dev/nor/config-parser.h>
#include <dev/spi/zynq-qspi-flash.h>
#include <dev/spi/zynq-qspi-flash-defs.h>
#include <dev/spi/jedec_flash.h>

/*
 * Debug tracing.
 */
#define ZQSPI_FLASH_TRACE_TRANSFER 0

#if ZQSPI_FLASH_TRACE_TRANSFER
static bool zqspi_trace = 1;
#endif

#if ZQSPI_FLASH_TRACE_TRANSFER
static void zqspi_transfer_trace_header(
  const char* message,
  const zqspi_transfer_buffer* transfer
)
{
  if (zqspi_trace)
    printf(" %s: length=%ld in=%ld out=%ld padding=%ld\n",
         message, transfer->length, transfer->in,
         transfer->out, transfer->padding);
}
#endif

void zqspi_transfer_trace(
  const char* message,
  const zqspi_transfer_buffer* transfer
)
{
#if ZQSPI_FLASH_TRACE_TRANSFER
  if (zqspi_trace)
  {
    size_t c;
    zqspi_transfer_trace_header(message, transfer);
    for (c = 0; c < transfer->length; ++c)
    {
      if ((c % 16) == 0)
      {
        if (c)
          printf("\n");
        printf("  %04lx ", c);
      }
      printf("%02x", transfer->buffer[c + transfer->padding]);
      if ((c % 16) == 7)
        printf("-");
      else
        printf(" ");
    }
    printf("\n");
  }
#else
  (void) message;
  (void) transfer;
#endif
}

void qspi_reg_write(uint32_t reg, uint32_t value)
{
  volatile uint32_t* ap = (uint32_t*)(ZQSPI_QSPI_BASE + reg);
  *ap = value;
}

uint32_t qspi_reg_read(uint32_t reg)
{
  volatile uint32_t* ap = (uint32_t*)(ZQSPI_QSPI_BASE + reg);
  return *ap;
}

static inline uint8_t zqspi_get8(const uint8_t* data)
{
  return *data;
}

static inline uint16_t zqspi_get16(const uint8_t* data)
{
  return (((uint16_t) data[1]) << 8) | data[0];
}

static void zqspi_transfer_buffer_clear(zqspi_transfer_buffer* transfer)
{
  transfer->length = 0;
  transfer->padding = 0;
  transfer->in = 0;
  transfer->out = 0;
}

static void zqspi_transfer_buffer_set_length(
  zqspi_transfer_buffer* transfer,
  size_t length
)
{
  transfer->length = length;
  transfer->padding = (4 - (length & 3)) & 3;
  transfer->in = transfer->padding;
  transfer->out = transfer->padding;
}

static zqspi_error zqspi_transfer_buffer_fill(
  zqspi_transfer_buffer* transfer,
  const uint8_t data,
  size_t length
)
{
  if ((transfer->in + length) >= transfer->size) {
    return ZQSPI_FLASH_BUFFER_OVERFLOW;
  }

  memset(transfer->buffer + transfer->in, data, length);

  transfer->in += length;

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_transfer_buffer_set8(
  zqspi_transfer_buffer* transfer,
  const uint8_t data
)
{
  if (transfer->in >= transfer->size) {
    return ZQSPI_FLASH_BUFFER_OVERFLOW;
  }

  volatile uint8_t* p = &transfer->buffer[transfer->in++];
  *p = data;

  return ZQSPI_FLASH_NO_ERROR;
}

zqspi_error zqspi_transfer_buffer_skip(
  zqspi_transfer_buffer* transfer,
  const size_t size
)
{
  if ((transfer->length - (transfer->out - transfer->padding)) < size) {
    return ZQSPI_FLASH_BUFFER_UNDERFLOW;
  }

  transfer->out += size;
  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_transfer_buffer_get8(
  zqspi_transfer_buffer* transfer,
  uint8_t* data
)
{
  if ((transfer->length - (transfer->out - transfer->padding)) <
        sizeof(uint8_t)) {
    return ZQSPI_FLASH_BUFFER_UNDERFLOW;
  }

  *data = transfer->buffer[transfer->out++];

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_transfer_buffer_get16(
  zqspi_transfer_buffer* transfer,
  uint16_t* data
)
{
  if ((transfer->length - (transfer->out - transfer->padding)) <
        sizeof(uint16_t)) {
    return ZQSPI_FLASH_BUFFER_UNDERFLOW;
  }

  *data = ((uint16_t) transfer->buffer[transfer->out++]) << 8;
  *data |= transfer->buffer[transfer->out++];

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_transfer_buffer_copy_out(
  zqspi_transfer_buffer* transfer,
  uint8_t* data,
  const size_t length
)
{
  if ((transfer->length - (transfer->out - transfer->padding)) < length) {
    return ZQSPI_FLASH_BUFFER_UNDERFLOW;
  }
  memcpy(data, transfer->buffer + transfer->out, length);
  transfer->out += length;

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_transfer_buffer_copy_in(
  zqspi_transfer_buffer* transfer,
  const uint8_t* data,
  const size_t length
)
{
  if ((transfer->in + length) > transfer->size) {
    return ZQSPI_FLASH_BUFFER_OVERFLOW;
  }

  memcpy(transfer->buffer + transfer->in, data, length);
  transfer->in += length;

  return ZQSPI_FLASH_NO_ERROR;
}

static void zqspi_transfer_buffer_set_addr(
  zqspi_transfer_buffer* transfer,
  bool addr_4_byte,
  uint32_t address
)
{
  if (addr_4_byte) {
    zqspi_transfer_buffer_set8(transfer, (address >> 24) & 0xff);
  }
  zqspi_transfer_buffer_set8(transfer, (address >> 16) & 0xff);
  zqspi_transfer_buffer_set8(transfer, (address >> 8) & 0xff);
  zqspi_transfer_buffer_set8(transfer, address & 0xff);
}

static void zqspi_transfer_buffer_set_dir(
  zqspi_transfer_buffer* transfer,
  int trans_dir
)
{
  transfer->trans_dir = trans_dir;
}

static void zqspi_transfer_buffer_set_command_len(
  zqspi_transfer_buffer* transfer,
  int comm_len
)
{
  transfer->command_len = comm_len;
}

static zqspi_error zqspi_read_register(
  zqspiflash *driver,
  uint8_t reg,
  uint16_t* value
)
{
  zqspi_error fe;

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE + 2);
  zqspi_transfer_buffer_set8(&driver->buf, reg);
  zqspi_transfer_buffer_set8(&driver->buf, 0);
  zqspi_transfer_buffer_set8(&driver->buf, 0);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_RX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR)
    return fe;

  zqspi_transfer_buffer_get16(&driver->buf, value);

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_wait_for_write(zqspiflash *driver, uint32_t wait)
{
  uint16_t status;
  zqspi_error fe;
  uint32_t checks = 100;

  while (true) {
    fe = zqspi_read_register(driver, ZQSPI_FLASH_READ_STATUS_FLAG_CMD, &status);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    status = status & 0xFF;

    if ((status & ZQSPI_FLASH_SR_E_ERR) != 0) {
      return ZQSPI_FLASH_ERASE_FAILURE;
    }

    /*
     * A succuessful write requires the write latch and the write in
     * progress have cleared. If the write in progress is not set yet the
     * write latch remains set it is an error and the write command was not
     * received the flash device.
     */
    fe = zqspi_read_register(driver, ZQSPI_FLASH_READ_STATUS_CMD, &status);
    status = status & 0xFF;
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    if ((status & ZQSPI_FLASH_SR_WIP) == 0)
    {
      if ((status & ZQSPI_FLASH_SR_WEL) == 0) {
        break;
      }
      if (checks == 0) {
        return ZQSPI_FLASH_WRITE_ERASE_CMD_FAIL;
      }
      --checks;
    }

    if (wait) {
      usleep(wait);
    }
  }
  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_set_WEL(zqspiflash *driver)
{
  uint16_t status;
  zqspi_error fe;

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);
  zqspi_transfer_buffer_set8(&driver->buf, ZQSPI_FLASH_WRITE_ENABLE_CMD);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_TX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR) {
    return fe;
  }

  fe = zqspi_read_register(driver, ZQSPI_FLASH_READ_STATUS_CMD, &status);
  if (fe != ZQSPI_FLASH_NO_ERROR) {
    return fe;
  }

  if ((status & ZQSPI_FLASH_SR_WEL) == 0) {
    return ZQSPI_FLASH_READ_ONLY;
  }

  return ZQSPI_FLASH_NO_ERROR;
}

static zqspi_error zqspi_clear_WEL(zqspiflash *driver) {
  uint16_t status;
  zqspi_error fe;

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);
  zqspi_transfer_buffer_set8(&driver->buf, ZQSPI_FLASH_WRITE_DISABLE_CMD);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_TX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR) {
    return fe;
  }

  fe = zqspi_read_register(driver, ZQSPI_FLASH_READ_STATUS_CMD, &status);
  if (fe != ZQSPI_FLASH_NO_ERROR) {
    return fe;
  }

  if ((status & ZQSPI_FLASH_SR_WEL) != 0) {
    return ZQSPI_FLASH_WRITE_LATCH_CLEAR_FAIL;
  }

  return ZQSPI_FLASH_NO_ERROR;
}

zqspi_error zqspi_read(
  zqspiflash *driver,
  uint32_t address,
  void* buffer,
  size_t length
)
{
  uint8_t* data = buffer;

  while (length)
  {
    zqspi_error fe;
    size_t    size;

    if (driver->flash_page_size == 0) {
      return ZQSPI_FLASH_INVALID_DEVICE;
    }
    size = length > driver->flash_page_size ? driver->flash_page_size : length;

    zqspi_transfer_buffer_clear(&driver->buf);
    zqspi_transfer_buffer_set_length(
      &driver->buf,
      ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3) +
        driver->flash_read_dummies + size
    );
    zqspi_transfer_buffer_set8(&driver->buf, driver->commands.fast_read);
    zqspi_transfer_buffer_set_addr(&driver->buf, driver->addr_4_byte,
        address);
    fe = zqspi_transfer_buffer_fill(&driver->buf, 0,
           driver->flash_read_dummies + size);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }
    zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_RX_TRANS);
    zqspi_transfer_buffer_set_command_len(&driver->buf,
        ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3) +
        driver->flash_read_dummies);

    fe = zqspi_transfer(&driver->buf, &driver->initialised);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    zqspi_transfer_buffer_skip(&driver->buf,
      (driver->addr_4_byte ? 4 : 3) + driver->flash_read_dummies);

    fe = zqspi_transfer_buffer_copy_out(&driver->buf, data, size);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    length -= size;
    data += size;
    address += size;
  }

  return ZQSPI_FLASH_NO_ERROR;
}

zqspi_error zqspi_blank(zqspiflash *driver, uint32_t address, size_t length)
{
  zqspi_error fe = ZQSPI_FLASH_NO_ERROR;

  if (
    (address >= driver->flash_size) ||
    ((address + length) > driver->flash_size)
  )
  {
    return ZQSPI_FLASH_BAD_ADDRESS;
  }

  while (length)
  {
    size_t    size;

    if (driver->flash_page_size == 0) {
      return ZQSPI_FLASH_INVALID_DEVICE;
    }
    size = length > driver->flash_page_size ? driver->flash_page_size : length;

    zqspi_transfer_buffer_clear(&driver->buf);
    zqspi_transfer_buffer_set_length(&driver->buf,
                     ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3)
                     + driver->flash_read_dummies + size);
    zqspi_transfer_buffer_set8(&driver->buf, driver->commands.fast_read);
    zqspi_transfer_buffer_set_addr(&driver->buf, driver->addr_4_byte,
        address);
    fe = zqspi_transfer_buffer_fill(&driver->buf, 0,
      driver->flash_read_dummies + size);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }
    zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_RX_TRANS);
    zqspi_transfer_buffer_set_command_len(&driver->buf,
      ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3)
      + driver->flash_read_dummies);

    fe = zqspi_transfer(&driver->buf, &driver->initialised);
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    zqspi_transfer_buffer_skip(&driver->buf, (driver->addr_4_byte ? 4 : 3));

    length -= size;
    address += size;

    while (size) {
      uint8_t byte = 0;
      zqspi_transfer_buffer_get8(&driver->buf, &byte);
      if (byte != 0xff) {
        return ZQSPI_FLASH_NOT_BLANK;
      }
      --size;
    }
  }

  return fe;
}

zqspi_error zqspi_erase(zqspiflash *driver, uint32_t address, size_t length)
{
  zqspi_error fe = ZQSPI_FLASH_NO_ERROR;
  bool done = false;

  if (
    (address >= driver->flash_size) ||
    ((address + length) > driver->flash_size)
  ) {
    return ZQSPI_FLASH_BAD_ADDRESS;
  }
  if (driver->flash_page_size == 0) {
    return ZQSPI_FLASH_INVALID_DEVICE;
  }

  while (!done) {
    fe = zqspi_erase_sector(driver,
           (address - (address%(driver->flash_erase_sector_size))));
    if (fe != ZQSPI_FLASH_NO_ERROR) {
      return fe;
    }

    if (length <= driver->flash_erase_sector_size) {
      done = true;
    } else {
      address += driver->flash_erase_sector_size;
      length -= driver->flash_erase_sector_size;
    }
  }
  return fe;
}

zqspi_error zqspi_erase_sector(zqspiflash *driver, uint32_t address)
{
  zqspi_error fe = ZQSPI_FLASH_NO_ERROR;
  uint32_t base = 0;
  size_t length = 0;

  if (
    (address >= driver->flash_size) ||
    ((address + length) > driver->flash_size)
  )
  {
    return ZQSPI_FLASH_BAD_ADDRESS;
  }

  zqspi_write_unlock(driver);

  fe = zqspi_set_WEL(driver);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_write_lock(driver);
    return fe;
  }

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE
    + (driver->addr_4_byte ? 4 : 3));
  zqspi_transfer_buffer_set8(&driver->buf, driver->commands.sector_erase);
  zqspi_transfer_buffer_set_addr(&driver->buf, driver->addr_4_byte,
      address);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_TX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE
    + (driver->addr_4_byte ? 4 : 3));

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_clear_WEL(driver);
    zqspi_write_lock(driver);
    return fe;
  }

  fe = zqspi_wait_for_write(driver, 1000);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_clear_WEL(driver);
    zqspi_write_lock(driver);
    return fe;
  }

  zqspi_write_lock(driver);

  fe = zqspi_blank(driver, base, length);
  if (fe != ZQSPI_FLASH_NO_ERROR)
    return fe;

  return fe;
}

zqspi_error zqspi_erase_device(zqspiflash *driver)
{
  zqspi_error fe = ZQSPI_FLASH_NO_ERROR;

  zqspi_write_unlock(driver);

  fe = zqspi_set_WEL(driver);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_write_lock(driver);
    return fe;
  }

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, 1);
  zqspi_transfer_buffer_set8(&driver->buf, ZQSPI_FLASH_BULK_ERASE_CMD);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_TX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf,
      ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3));

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_clear_WEL(driver);
    zqspi_write_lock(driver);
    return fe;
  }

  fe = zqspi_wait_for_write(driver, 1000000);
  if (fe != ZQSPI_FLASH_NO_ERROR)
  {
    zqspi_clear_WEL(driver);
    zqspi_write_lock(driver);
    return fe;
  }

  zqspi_write_lock(driver);

  return fe;
}

zqspi_error zqspi_write(
  zqspiflash *driver,
  uint32_t address,
  const void* buffer,
  size_t length
)
{
  zqspi_error fe = ZQSPI_FLASH_NO_ERROR;
  const uint8_t* data = buffer;
  size_t size;
  size_t byte;
  bool write_block = false;

  if (
    (address >= driver->flash_size) ||
    ((address + length) > driver->flash_size)
  )
  {
    return ZQSPI_FLASH_BAD_ADDRESS;
  }

  zqspi_write_unlock(driver);

  while (length) {
    if (driver->flash_page_size == 0) {
      return ZQSPI_FLASH_INVALID_DEVICE;
    }
    size = length > driver->flash_page_size ? driver->flash_page_size : length;

    /*
     * If the address is not aligned to the start of a page transfer
     * the remainder of the page so the address is aligned.
     */
    if ((address % driver->flash_page_size) != 0)
    {
      size_t remaining = driver->flash_page_size
        - (address % driver->flash_page_size);
      if (size > remaining)
        size = remaining;
    }

    /*
     * If the block is blank do not write it.
     */
    for (byte = 0; byte < size; ++byte)
    {
      if (data[byte] != 0xff)
      {
        write_block = true;
        break;
      }
    }

    if (write_block)
    {
      fe = zqspi_set_WEL(driver);
      if (fe != ZQSPI_FLASH_NO_ERROR)
      {
        zqspi_write_lock(driver);
        return fe;
      }

      zqspi_transfer_buffer_clear(&driver->buf);
      zqspi_transfer_buffer_set_length(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE
          + (driver->addr_4_byte ? 4 : 3) + size);
      zqspi_transfer_buffer_set8(&driver->buf, driver->commands.page_program);
      zqspi_transfer_buffer_set_addr(&driver->buf, driver->addr_4_byte,
          address);
      zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_TX_TRANS);
      zqspi_transfer_buffer_set_command_len(&driver->buf,
        ZQSPI_FLASH_COMMAND_SIZE + (driver->addr_4_byte ? 4 : 3));
      fe = zqspi_transfer_buffer_copy_in(&driver->buf, data, size);
      if (fe != ZQSPI_FLASH_NO_ERROR)
      {
        zqspi_write_lock(driver);
        return fe;
      }

      fe = zqspi_transfer(&driver->buf, &driver->initialised);
      if (fe != ZQSPI_FLASH_NO_ERROR)
      {
        zqspi_clear_WEL(driver);
        zqspi_write_lock(driver);
        return fe;
      }

      fe = zqspi_wait_for_write(driver, 1000);
      if (fe != ZQSPI_FLASH_NO_ERROR)
      {
        zqspi_clear_WEL(driver);
        zqspi_write_lock(driver);
        return fe;
      }
    }

    address += size;
    data += size;
    length -= size;
  }

  zqspi_write_lock(driver);

  return fe;
}

zqspi_error zqspi_readid(zqspiflash *driver, uint32_t *jedec_id)
{
  zqspi_error fe;
  uint8_t value = 0;
  int index = 0;
  bool use_cfi = true;
  size_t cfi_raw_len = 0;

  if (driver->jedec_id != 0) {
    if (jedec_id != NULL) {
      *jedec_id = driver->jedec_id;
    }
    return ZQSPI_FLASH_NO_ERROR;
  }

  zqspi_transfer_buffer_clear(&driver->buf);
  zqspi_transfer_buffer_set_length(&driver->buf, 1 + 4);
  zqspi_transfer_buffer_set8(&driver->buf, ZQSPI_FLASH_READ_ID);
  zqspi_transfer_buffer_fill(&driver->buf, 0x00, 4);
  zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_RX_TRANS);
  zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);

  fe = zqspi_transfer(&driver->buf, &driver->initialised);
  if (fe != ZQSPI_FLASH_NO_ERROR)
    return fe;

  zqspi_transfer_buffer_get8(&driver->buf, &value);
  driver->jedec_id |= value << 16;
  zqspi_transfer_buffer_get8(&driver->buf, &value);
  driver->jedec_id |= value << 8;
  zqspi_transfer_buffer_get8(&driver->buf, &value);
  driver->jedec_id |= value;

  zqspi_transfer_buffer_get8(&driver->buf, &value);
  cfi_raw_len = value + 4;

  if (jedec_id != NULL) {
    *jedec_id = driver->jedec_id;
  }

  while (flash_dev_table[index].jedec_id != 0) {
    if (flash_dev_table[index].jedec_id == driver->jedec_id) {
      use_cfi = false;
      driver->flash_size = flash_dev_table[index].flash_size;
      driver->flash_erase_sector_size = flash_dev_table[index].sec_size;
      driver->flash_page_size = flash_dev_table[index].page_size;
      break;
    }
    index++;
  }

  if (use_cfi) {
    rtems_status_code sc;
    rtems_flash_NOR_config_data nor_config;
    uint8_t cfi_data[256];

    if (cfi_raw_len > sizeof(cfi_data)) {
      return ZQSPI_FLASH_INVALID_DEVICE;
    }

    zqspi_transfer_buffer_clear(&driver->buf);
    zqspi_transfer_buffer_set_length(&driver->buf, 1 + cfi_raw_len);
    zqspi_transfer_buffer_set8(&driver->buf, ZQSPI_FLASH_READ_ID);
    zqspi_transfer_buffer_fill(&driver->buf, 0x00, cfi_raw_len);
    zqspi_transfer_buffer_set_dir(&driver->buf, ZQSPI_FLASH_RX_TRANS);
    zqspi_transfer_buffer_set_command_len(&driver->buf, ZQSPI_FLASH_COMMAND_SIZE);

    fe = zqspi_transfer(&driver->buf, &driver->initialised);
    if (fe != ZQSPI_FLASH_NO_ERROR)
      return fe;

    fe = zqspi_transfer_buffer_copy_out(&driver->buf, cfi_data, cfi_raw_len);
    if (fe != ZQSPI_FLASH_NO_ERROR)
      return fe;

    sc = rtems_flash_CFI_parse_from_buffer(cfi_data, cfi_raw_len, &nor_config);
    if (sc != RTEMS_SUCCESSFUL) {
      return ZQSPI_FLASH_INVALID_DEVICE;
    }

    driver->flash_size = nor_config.device_size;
    driver->flash_erase_sector_size = nor_config.sector_size;
    driver->flash_page_size = nor_config.page_size;
  }

  if (driver->flash_size > 16 * 1024 * 1024) {
    driver->addr_4_byte = true;
    driver->commands.fast_read = ZQSPI_FLASH_4B_FAST_READ_CMD;
    driver->commands.read = ZQSPI_FLASH_4B_READ_CMD;
    driver->commands.page_program = ZQSPI_FLASH_4B_WRITE_CMD;
    driver->commands.sector_erase = ZQSPI_FLASH_4B_SEC_ERASE_CMD;
  }

  return ZQSPI_FLASH_NO_ERROR;
}

size_t zqspi_device_size(zqspiflash *driver)
{
  return driver->flash_size;
}

size_t zqspi_device_sector_erase_size(zqspiflash *driver)
{
  return driver->flash_erase_sector_size;
}

zqspi_error zqspi_init(zqspiflash *driver)
{
  rtems_status_code sc;
  uint8_t *zqspizqspi_buf = calloc(1, ZQSPI_FLASH_BUFFER_SIZE);

  driver->buf.size = ZQSPI_FLASH_BUFFER_SIZE;
  driver->buf.length = 0;
  driver->buf.padding = 0;
  driver->buf.in = 0;
  driver->buf.out = 0;
  driver->buf.trans_dir = 0;
  driver->buf.command_len = 0;
  driver->buf.buffer = zqspizqspi_buf;
  driver->buf.tx_data = NULL;
  driver->buf.rx_data = NULL;
  driver->buf.tx_length = 0;
  driver->buf.rx_length = 0;
  driver->buf.sending = 0;
  driver->buf.start = false;
  driver->initialised = false;
  driver->addr_4_byte = false;
  driver->jedec_id = 0;
  driver->flash_size = 0;
  driver->flash_read_dummies = 1;
  driver->flash_erase_sector_size = 0;
  driver->flash_page_size = 0;
  driver->commands.fast_read = ZQSPI_FLASH_3B_FAST_READ_CMD;
  driver->commands.read = ZQSPI_FLASH_3B_READ_CMD;
  driver->commands.page_program = ZQSPI_FLASH_3B_WRITE_CMD;
  driver->commands.sector_erase = ZQSPI_FLASH_3B_SEC_ERASE_CMD;

  sc = rtems_interrupt_handler_install(
    ZQPSI_ZYNQ_QSPI_IRQ,
    NULL,
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) zqspi_transfer_intr,
    driver
  );
  if (sc != RTEMS_SUCCESSFUL) {
    free(driver->buf.buffer);
    return ZQSPI_FLASH_RTEMS_INTR;
  }

  return zqspi_readid(driver, NULL);
}

void zqspi_close(zqspiflash *driver)
{
  free(driver->buf.buffer);
}
