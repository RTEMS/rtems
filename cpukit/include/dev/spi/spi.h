/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Serial Peripheral Interface (SPI) Driver API
 *
 * @ingroup SPI
 */

/*
 * Copyright (C) 2016, 2017 embedded brains GmbH & Co. KG
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

#ifndef _DEV_SPI_SPI_H
#define _DEV_SPI_SPI_H

#include <linux/spi/spidev.h>

#include <rtems.h>
#include <rtems/seterr.h>
#include <rtems/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct spi_ioc_transfer spi_ioc_transfer;

typedef struct spi_bus spi_bus;

/**
 * @defgroup SPI Serial Peripheral Interface (SPI) Driver
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Serial Peripheral Interface (SPI) bus driver support.
 *
 * @{
 */

/**
 * @brief Obtains the bus.
 *
 * This command has no argument.
 */
#define SPI_BUS_OBTAIN _IO(SPI_IOC_MAGIC, 13)

/**
 * @brief Releases the bus.
 *
 * This command has no argument.
 */
#define SPI_BUS_RELEASE _IO(SPI_IOC_MAGIC, 23)

/**
 * @brief SPI bus control.
 */
struct spi_bus {
  /**
   * @brief Transfers SPI messages.
   *
   * @param[in] bus The bus control.
   * @param[in] msgs The messages to transfer.
   * @param[in] msg_count The count of messages to transfer.  It must be
   * positive.
   *
   * @retval 0 Successful operation.
   * @retval negative Negative error number in case of an error.
   */
  int (*transfer)(spi_bus *bus, const spi_ioc_transfer *msgs, uint32_t msg_count);

  /**
   * @brief Checks if maximum speed and bits per word are in a valid range
   * for the device
   *
   * @param[in] bus The bus control.
   *
   * @retval 0 Successful operation.
   * @retval negative Negative error number in case of an error.
   */
  int (*setup)(spi_bus *bus);

  /**
   * @brief Destroys the bus.
   *
   * @param[in] bus The bus control.
   */
  void (*destroy)(spi_bus *bus);

  /**
   * @brief Mutex to protect the bus access.
   */
  rtems_recursive_mutex mutex;

  /**
   * @brief Maximum Speed in Hz
   */
  uint32_t max_speed_hz;

  /**
   * @brief Indicates the speed of the current device message.
   */
  uint32_t speed_hz;

  /**
   * @brief Indicates if chip select must be set high after transfer.
   */
  bool cs_change;

  /**
   * @brief Indicates which device is selected by chip select
   */
  uint8_t cs;

  /**
   * @brief Indicates the bits per word used on the device.
   */
  uint8_t bits_per_word;

  /**
   * @brief Indicates if LSB is supposed to be transmitted first.
   */
  bool lsb_first;

  /**
   * @brief Current mode.
   */
  uint32_t mode;

  /**
   * @brief Indicates the delay between transfers on different chip select
   * devices.
   */
  uint16_t delay_usecs;

  /**
   * @brief Driver specific ioctl.
   *
   * @param[in] bus The bus control.
   */
  int (*ioctl)(spi_bus *bus, ioctl_command_t command, void *arg);
};

/**
 * @brief Initializes a bus control.
 *
 * After a sucessful initialization the bus control must be destroyed via
 * spi_bus_destroy().  A registered bus control will be automatically destroyed
 * in case the device file is unlinked.  Make sure to call spi_bus_destroy() in
 * a custom destruction handler.
 *
 * @param[in] bus The bus control.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 *
 * @see spi_bus_register()
 */
int spi_bus_init(spi_bus *bus);

/**
 * @brief Allocates a bus control from the heap and initializes it.
 *
 * After a sucessful allocation and initialization the bus control must be
 * destroyed via spi_bus_destroy_and_free().  A registered bus control will be
 * automatically destroyed in case the device file is unlinked.  Make sure to
 * call spi_bus_destroy_and_free() in a custom destruction handler.
 *
 * @param[in] size The size of the bus control.  This enables the addition of
 * bus controller specific data to the base bus control.  The bus control is
 * zero initialized.
 *
 * @retval non-NULL The new bus control.
 * @retval NULL An error occurred.  The errno is set to indicate the error.
 *
 * @see spi_bus_register()
 */
spi_bus *spi_bus_alloc_and_init(size_t size);

/**
 * @brief Destroys a bus control.
 *
 * @param[in] bus The bus control.
 */
void spi_bus_destroy(spi_bus *bus);

/**
 * @brief Destroys a bus control and frees its memory.
 *
 * @param[in] bus The bus control.
 */
void spi_bus_destroy_and_free(spi_bus *bus);

/**
 * @brief Registers a bus control.
 *
 * This function claims ownership of the bus control regardless if the
 * registration is successful or not.
 *
 * @param[in] bus The bus control.
 * @param[in] bus_path The path to the bus device file.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The errno is set to indicate the error.
 */
int spi_bus_register(
  spi_bus *bus,
  const char *bus_path
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_SPI_SPI_H */
