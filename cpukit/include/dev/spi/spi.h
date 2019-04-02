/**
 * @file
 *
 * @brief Serial Peripheral Interface (SPI) Driver API
 *
 * @ingroup SPI
 */

/*
 * Copyright (c) 2016, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
