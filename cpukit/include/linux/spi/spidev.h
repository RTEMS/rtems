/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Port of Linux SPI API
 *
 * @ingroup SPILinux
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#ifndef _UAPI_LINUX_SPI_H
#define _UAPI_LINUX_SPI_H

#include <sys/ioccom.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup SPILinux Linux SPI User-Space API
 *
 * @ingroup SPI
 *
 * @brief RTEMS port of Linux SPI user-space API.
 *
 * Additional documentation is available through the Linux sources, see
 *
 * /usr/src/linux/include/uapi/linux/spidev.h.
 *
 * @{
 */

/**
 * @name SPI Transfer Flags
 *
 * @{
 */

/**
 * @brief SPI transfer flag which sets the clock phase.
 */
#define SPI_CPHA 0x01

/**
 * @brief SPI transfer flag which sets the clock polarity.
 */
#define SPI_CPOL 0x02

/**
 * @brief SPI transfer flag which sets SPI Mode 0 (clock starts low, sample on
 * leading edge).
 */
#define SPI_MODE_0 0

/**
 * @brief SPI transfer flag which sets SPI Mode 0 (clock starts low, sample on
 * trailing edge).
 */
#define SPI_MODE_1 SPI_CPHA

/**
 * @brief SPI transfer flag which sets SPI Mode 0 (clock starts high, sample on
 * leading edge).
 */
#define SPI_MODE_2 SPI_CPOL

/**
 * @brief SPI transfer flag which sets SPI Mode 0 (clock starts high, sample on
 * trailing edge).
 */
#define SPI_MODE_3 (SPI_CPOL | SPI_CPHA)

/**
 * @brief SPI transfer flag which selects the device by setting the chip select
 * line.
 */
#define SPI_CS_HIGH 0x04

/**
 * @brief SPI transfer flag which triggers data transmission with the LSB being
 * sent first.
 */
#define SPI_LSB_FIRST 0x08

/**
 * @brief SPI transfer flag which uses a shared wire for master input/slave
 * output as well as master output/slave input.
 */
#define SPI_3WIRE 0x10

/**
 * @brief SPI transfer flag which initiates the loopback mode.
 */
#define SPI_LOOP 0x20

/**
 * @brief SPI transfer flag which indicates that no chip select is needed due to
 * only one device on the bus.
 */
#define SPI_NO_CS 0x40

/**
 * @brief SPI transfer flag which pulls the slave to low level during pause.
 */
#define SPI_READY 0x80

/**
 * @brief SPI transfer flag which sets up dual mode for transmission.
 */
#define SPI_TX_DUAL 0x100

/**
 * @brief SPI transfer flag which sets up quad mode for transmission.
 */
#define SPI_TX_QUAD 0x200

/**
 * @brief SPI transfer flag which sets up dual mode for reception.
 */
#define SPI_RX_DUAL 0x400

/**
 * @brief SPI transfer flag which sets up quad mode for reception.
 */
#define SPI_RX_QUAD 0x800

/** @} */

#define SPI_IOC_MAGIC 's'

/**
 * @brief SPI transfer message.
 */
struct spi_ioc_transfer {
  /**
   * @brief Buffer for receive data.
   */
  void *rx_buf;

  /**
   * @brief Buffer for transmit data.
   */
  const void *tx_buf;

  /**
   * @brief Length of receive and transmit buffers in bytes.
   */
  size_t len;

  /**
   * @brief Sets the bit-rate of the device.
   */
  uint32_t speed_hz;

  /**
   * @brief Sets the delay after a transfer before the chip select status is
   * changed and the next transfer is triggered.
   */
  uint16_t delay_usecs;

  /**
   * @brief Sets the device wordsize.
   */
  uint8_t bits_per_word;

  /**
   * @brief If true, device is deselected after transfer ended and before a new
   * transfer is started.
   */
  uint8_t cs_change;

  /**
   * @brief Amount of bits that are used for reading.
   */
  uint8_t rx_nbits;

  /**
   * @brief Amount of bits that are used for writing.
   */
  uint8_t tx_nbits;

  /**
   * @brief Sets one of the possible modes that can be used for SPI transfers
   * (dependent on clock phase and polarity).
   */
  uint32_t mode;

  /**
   * @brief Indicates which device is currently used.
   */
  uint8_t cs;
};

/**
 * @brief Calculates the size of the SPI message array.
 */
#define SPI_MSGSIZE(n) \
  (((n) * sizeof(struct spi_ioc_transfer) < IOCPARM_MAX) ? \
    (n) * sizeof(struct spi_ioc_transfer) : 0)

/**
 * @brief Transfers an array with SPI messages.
 */
#define SPI_IOC_MESSAGE(n) _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(n)])

/**
 * @brief Reads the least-significant 8-bits of the SPI default mode.
 */
#define SPI_IOC_RD_MODE _IOR(SPI_IOC_MAGIC, 1, uint8_t)

/**
 * @brief Writes the SPI default mode (the most-significant 24-bits of the mode are
 * set to zero).
 */
#define SPI_IOC_WR_MODE _IOW(SPI_IOC_MAGIC, 1, uint8_t)

/**
 * @brief Reads the SPI default least-significant bit first setting.
 */
#define SPI_IOC_RD_LSB_FIRST _IOR(SPI_IOC_MAGIC, 2, uint8_t)

/**
 * @brief Writes the SPI default least-significant-bit first setting.
 */
#define SPI_IOC_WR_LSB_FIRST _IOW(SPI_IOC_MAGIC, 2, uint8_t)

/**
 * @brief Reads the SPI default bits per word.
 */
#define SPI_IOC_RD_BITS_PER_WORD _IOR(SPI_IOC_MAGIC, 3, uint8_t)

/**
 * @brief Writes the SPI default bits per word.
 */
#define SPI_IOC_WR_BITS_PER_WORD _IOW(SPI_IOC_MAGIC, 3, uint8_t)

/**
 * @brief Reads the SPI default speed in Hz.
 */
#define SPI_IOC_RD_MAX_SPEED_HZ _IOR(SPI_IOC_MAGIC, 4, uint32_t)

/**
 * @brief Writes the SPI default speed in Hz.
 */
#define SPI_IOC_WR_MAX_SPEED_HZ _IOW(SPI_IOC_MAGIC, 4, uint32_t)

/**
 * @brief Reads the full 32-bit SPI default mode.
 */
#define SPI_IOC_RD_MODE32 _IOR(SPI_IOC_MAGIC, 5, uint32_t)

/**
 * @brief Writes the full 32-bit SPI default mode.
 */
#define SPI_IOC_WR_MODE32 _IOW(SPI_IOC_MAGIC, 5, uint32_t)

#endif /* _UAPI_LINUX_SPI_H */
