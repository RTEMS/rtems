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

#if !defined(_ZYNQ_QSPI_FLASH_H_)
#define _ZYNQ_QSPI_FLASH_H_

#include <stdbool.h>
#include <stdlib.h>

/*
 * Driver configuration.
 */
#define ZQSPI_FLASH_COMMAND_OFFSET    (0) /* FLASH instruction */
#define ZQSPI_FLASH_ADDRESS_1_OFFSET  (1) /* Bits 31-24 of the address */
#define ZQSPI_FLASH_ADDRESS_2_OFFSET  (2) /* Bits 23-16 of the address */
#define ZQSPI_FLASH_ADDRESS_3_OFFSET  (3) /* Bits 16-8 of the address */
#define ZQSPI_FLASH_ADDRESS_4_OFFSET  (4) /* Bits 8-0 of the address */
#define ZQSPI_FLASH_SPI_MAX_PADDING   (4) /* Maximum amount of padding. */

#define ZQSPI_FLASH_TX_TRANS 0
#define ZQSPI_FLASH_RX_TRANS 1

#define ZQSPI_FLASH_BUFFER_SIZE 0x1008

#define ZQPSI_ZYNQ_QSPI_IRQ 51

#define ZQSPI_TIMEOUT_US   5000000U
#define ZQSPI_TIMEOUT_TICKS (ZQSPI_TIMEOUT_US \
    / rtems_configuration_get_microseconds_per_tick())


/*
 * Zynq QSPI Flash driver errors.
 */
typedef enum
{
  ZQSPI_FLASH_NO_ERROR = 0,
  ZQSPI_FLASH_NOT_OPEN,
  ZQSPI_FLASH_ALREADY_OPEN,
  ZQSPI_FLASH_NO_MEMORY,
  ZQSPI_FLASH_4BYTE_ADDR_NOT_SUPPORTED,
  ZQSPI_FLASH_BUFFER_OVERFLOW,
  ZQSPI_FLASH_BUFFER_UNDERFLOW,
  ZQSPI_FLASH_BAD_ADDRESS,
  ZQSPI_FLASH_NOT_BLANK,
  ZQSPI_FLASH_ERASE_FAILURE,
  ZQSPI_FLASH_READ_ONLY,
  ZQSPI_FLASH_WRITE_LATCH_CLEAR_FAIL,
  ZQSPI_FLASH_WRITE_LOCK_FAIL,
  ZQSPI_FLASH_WRITE_ACROSS_SECTION,
  ZQSPI_FLASH_WRITE_ERASE_CMD_FAIL,
  ZQSPI_FLASH_LOCK_FAIL,
  ZQSPI_FLASH_INVALID_DEVICE,
  ZQPSI_FLASH_TRANSFER_FAILED,
  ZQSPI_FLASH_RTEMS_INTR
} zqspi_error;

/*
 * A transfer buffer.
 */
typedef struct
{
  size_t    size;
  size_t    length;
  size_t    padding;
  size_t    in;
  size_t    out;
  uint32_t  trans_dir;
  uint32_t  command_len;
  uint8_t   *buffer;
  uint32_t  *tx_data;
  uint32_t  *rx_data;
  size_t    tx_length;
  size_t    rx_length;
  size_t    sending;
  bool      start;
} zqspi_transfer_buffer;

typedef struct
{
  uint8_t fast_read;
  uint8_t read;
  uint8_t page_program;
  uint8_t sector_erase;
} zqspi_flash_commands;

/*
 * A flash driver instance
 */
typedef struct
{
  zqspi_transfer_buffer buf;
  zqspi_flash_commands  commands;
  bool                  initialised;
  bool                  addr_4_byte;
  uint32_t              jedec_id;
  uint64_t              flash_size;
  uint32_t              flash_read_dummies;
  uint32_t              flash_erase_sector_size;
  uint32_t              flash_page_size;
} zqspiflash;

/**
 * @brief Initialize zqspiflash driver.
 *
 * @param zqspiflash Device context.
 *
 * @retval zqspi_error code.
 */
zqspi_error zqspi_init(zqspiflash *driver);

/**
 * @brief Close zqspiflash driver.
 *
 * @param zqspiflash Device context.
 */
void zqspi_close(zqspiflash *driver);

/**
 * @brief Read from zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address to read from.
 * @param buffer Buffer to read data into.
 * @param length Length of requested read.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_read(
  zqspiflash *driver,
  uint32_t address,
  void* buffer,
  size_t length
);

/**
 * @brief Write to zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address to write to.
 * @param buffer Buffer to write data from.
 * @param length Length of requested write.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_write(
  zqspiflash *driver,
  uint32_t address,
  const void* buffer,
  size_t length
);

/**
 * @brief Write to zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address to erase.
 * @param length Length of requested erase.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_erase(
  zqspiflash *driver,
  uint32_t address,
  size_t length
);

/**
 * @brief Blank check zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address to check.
 * @param length Length of requested check.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_blank(
  zqspiflash *driver,
  uint32_t address,
  size_t length
);

/**
 * @brief Erase sector of zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address of sector to erase.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_erase_sector(
  zqspiflash *driver,
  uint32_t address
);

/**
 * @brief Write sector to zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param address Address to write to.
 * @param length Length of requested write.
 *
 * @retval zqspi_error Error code.
 */
zqspi_error zqspi_write_sector(
  zqspiflash *driver,
  uint32_t address,
  const void* buffer,
  size_t length
);

/**
 * @brief Erases all of zqspiflash.
 *
 * @param zqspiflash Device context.
 *
 * @retval zqspi_error error code.
 */
zqspi_error zqspi_erase_device(zqspiflash *driver);

/**
 * @brief Get JEDEC ID of zqspiflash.
 *
 * @param zqspiflash Device context.
 * @param jedec_id Pounter to uin32_t which the JEDEC ID is
 * returned in.
 *
 * @retval zqspi_error error code.
 */
zqspi_error zqspi_readid(zqspiflash *driver, uint32_t *jedec_id);

/**
 * @brief Get JEDEC ID of zqspiflash.
 *
 * @param zqspiflash Device context.
 *
 * @retval size Size of flash.
 */
size_t zqspi_device_size(zqspiflash *driver);

/**
 * @brief Get size of erase sectors of zqspiflash.
 *
 * @param zqspiflash Device context.
 *
 * @retval size Size of erase sector of flash.
 */
size_t zqspi_device_sector_erase_size(zqspiflash *driver);

/**
 * @brief If debug enabled prints trace of flash transfer.
 *
 * @param message Trace header.
 * @param transfer Transfer to print.
 */
void zqspi_transfer_trace(
  const char* message,
  const zqspi_transfer_buffer* transfer
);

/**
 * @brief Writes a uint32_t to a ZYNQ7000 QSPI register.
 *
 * @param reg Register offset.
 * @param value Value to write.
 */
void qspi_reg_write(uint32_t reg, uint32_t value);

/**
 * @brief Read a uint32_t from a ZYNQ7000 QSPI register.
 *
 * @param reg Register offset.
 *
 * @retval value Value read from register.
 */
uint32_t qspi_reg_read(uint32_t reg);

/**
 * @brief Skip value in transfer buffer.
 *
 * @param transfer Buffer work on.
 * @param size Number of bytes to skip.
 *
 * @retval zqspi_error error code.
 */
zqspi_error zqspi_transfer_buffer_skip(
  zqspi_transfer_buffer* transfer,
  const size_t size
);

#endif /* _ZYNQ_QSPI_FLASH_H_ */
