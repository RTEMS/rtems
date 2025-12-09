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

#if !defined(_ZYNQ_QSPI_FLASH_DEFS_H_)
#define _ZYNQ_QSPI_FLASH_DEFS_H_

#include <dev/spi/zynq-qspi-flash.h>

#define ZQSPI_QSPI_BASE 0xE000D000

/*
 * Flash commands.
 */

#define ZQSPI_FLASH_COMMAND_SIZE      1

#define ZQSPI_FLASH_4B_WRITE_CMD      0x12
#define ZQSPI_FLASH_4B_FAST_READ_CMD  0x0C
#define ZQSPI_FLASH_4B_READ_CMD       0x13
#define ZQSPI_FLASH_4B_SEC_ERASE_CMD  0xDC

#define ZQSPI_FLASH_3B_WRITE_CMD      0x02
#define ZQSPI_FLASH_3B_FAST_READ_CMD  0x0B
#define ZQSPI_FLASH_3B_READ_CMD       0x03
#define ZQSPI_FLASH_3B_SEC_ERASE_CMD  0xD8

#define ZQSPI_FLASH_READ_CONFIG_CMD       0x35
#define ZQSPI_FLASH_WRITE_STATUS_CMD      0x01
#define ZQSPI_FLASH_WRITE_DISABLE_CMD     0x04
#define ZQSPI_FLASH_READ_STATUS_CMD       0x05
#define ZQSPI_FLASH_WRITE_DISABLE_CMD     0x04
#define ZQSPI_FLASH_WRITE_ENABLE_CMD      0x06
#define ZQSPI_FLASH_BULK_ERASE_CMD        0xC7
#define ZQSPI_FLASH_READ_ID               0x9F

#define ZQSPI_FLASH_READ_STATUS_FLAG_CMD  0x05

/*
 * QSPI registers.
 */
#define ZQSPI_QSPI_REG_CONFIG           0x00000000
#define ZQSPI_QSPI_REG_INTR_STATUS      0x00000004
#define ZQSPI_QSPI_REG_INTR_ENABLE      0x00000008
#define ZQSPI_QSPI_REG_INTR_DISABLE     0x0000000c
#define ZQSPI_QSPI_REG_INTR_MASK        0x00000010
#define ZQSPI_QSPI_REG_EN               0x00000014
#define ZQSPI_QSPI_REG_DELAY            0x00000018
#define ZQSPI_QSPI_REG_TXD0             0x0000001c
#define ZQSPI_QSPI_REG_RX_DATA          0x00000020
#define ZQSPI_QSPI_REG_SLAVE_IDLE_COUNT 0x00000024
#define ZQSPI_QSPI_REG_TX_THRES         0x00000028
#define ZQSPI_QSPI_REG_RX_THRES         0x0000002c
#define ZQSPI_QSPI_REG_GPIO             0x00000030
#define ZQSPI_QSPI_REG_LPBK_DLY_ADJ     0x00000038
#define ZQSPI_QSPI_REG_TXD1             0x00000080
#define ZQSPI_QSPI_REG_TXD2             0x00000084
#define ZQSPI_QSPI_REG_TXD3             0x00000088
#define ZQSPI_QSPI_REG_LSPI_CFG         0x000000a0
#define ZQSPI_QSPI_REG_LSPI_STS         0x000000a4
#define ZQSPI_QSPI_REG_MOD_ID           0x000000fc

/*
 * TX FIFO depth in words.
 */
#define ZQSPI_QSPI_FIFO_DEPTH   (63)

/*
 * Control register.
 */
#define ZQSPI_QSPI_CR_HOLDB_DR        (1 << 19)
#define ZQSPI_QSPI_CR_MANSTRT         (1 << 16)
#define ZQSPI_QSPI_CR_MANSTRTEN       (1 << 15)
#define ZQSPI_QSPI_CR_SSFORCE         (1 << 14)
#define ZQSPI_QSPI_CR_PCS             (1 << 10)
#define ZQSPI_QSPI_CR_BAUD_RATE_DIV_2 (0 << 3)
#define ZQSPI_QSPI_CR_BAUD_RATE_DIV_4 (1 << 3)
#define ZQSPI_QSPI_CR_BAUD_RATE_DIV_8 (2 << 3)
#define ZQSPI_QSPI_CR_MODE_SEL        (1 << 0)

/*
 * Fast clock rate of 100MHz for fast reads.
 */
#define ZQSPI_QSPI_CR_BAUD_RATE_FAST ZQSPI_QSPI_CR_BAUD_RATE_DIV_2

/*
 * Status register.
 */
#define ZQSPI_QSPI_IXR_TXUF      (1 << 6)
#define ZQSPI_QSPI_IXR_RXFULL    (1 << 5)
#define ZQSPI_QSPI_IXR_RXNEMPTY  (1 << 4)
#define ZQSPI_QSPI_IXR_TXFULL    (1 << 3)
#define ZQSPI_QSPI_IXR_TXOW      (1 << 2)
#define ZQSPI_QSPI_INTR_RXOVR    (1 << 0)

/*
 * Enable register.
 */
#define ZQSPI_QSPI_EN_SPI_ENABLE  (1 << 0)

/*
 * Clock rate is 200MHz and 50MHz is the normal rate and 100MHz the fast rate.
 */
#if FLASH_FAST_READ
 #define ZQSPI_QSPI_CR_BAUD_RATE ZQSPI_QSPI_CR_BAUD_RATE_DIV_2
#else
 #define ZQSPI_QSPI_CR_BAUD_RATE ZQSPI_QSPI_CR_BAUD_RATE_DIV_4
#endif

/*
 * Flash Status bits.
 */
#define ZQSPI_FLASH_SR_WIP   (1 << 0)
#define ZQSPI_FLASH_SR_WEL   (1 << 1)
#define ZQSPI_FLASH_SR_BP0   (1 << 2)
#define ZQSPI_FLASH_SR_BP1   (1 << 3)
#define ZQSPI_FLASH_SR_BP2   (1 << 4)
#define ZQSPI_FLASH_SR_E_ERR (1 << 5)
#define ZQSPI_FLASH_SR_P_ERR (1 << 6)
#define ZQSPI_FLASH_SR_SRWD  (1 << 7)


void zqspi_write_unlock(zqspiflash *driver);

void zqspi_write_lock(zqspiflash *driver);

zqspi_error zqspi_transfer(zqspi_transfer_buffer* transfer, bool *initialised);

void zqspi_transfer_intr(zqspiflash *driver);

#endif /* _ZYNQ_QSPI_FLASH_DEFS_H_ */
