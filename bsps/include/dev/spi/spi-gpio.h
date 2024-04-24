/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceSPIGPIO
 *
 * @brief This header file provides the interfaces of @ref RTEMSDeviceSPIGPIO.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_DEV_SPI_SPI_GPIO_H
#define _RTEMS_DEV_SPI_SPI_GPIO_H

#include <rtems.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSDeviceSPIGPIO
 *
 * @ingroup SPI
 *
 * @brief A simple SPI bus driver that just uses GPIO pins
 *
 * Implements a SPI that is emulated by toggling GPIO pins. Callbacks are used
 * to set or get the pins so that it can be easily adapted to various GPIO
 * controllers.
 *
 * NOTE: This driver is slow! If you need performance: Don't use it. The
 * intended use case is (for example) a one time configuration of some SPI
 * peripheral.
 *
 * The driver will just work as fast as it can. Setting a speed limit is
 * currently not supported.
 *
 * @{
 */

#define SPI_GPIO_MAX_CS 4

/**
 * @brief Type of the functions that set pins
 *
 * Set a GPIO pin to @a level. @a level is false for low or true for high. @a
 * arg is an application specific parameter.
 */
typedef void (spi_gpio_set_pin_fn) (void *arg, bool level);

/**
 * @brief Type of the functions that read pins
 *
 * Get current level of GPIO pin. Should return either 0 for low or anything
 * else for high. @a arg is an application specific parameter.
 */
typedef bool (spi_gpio_get_pin_fn) (void *arg);

/**
 * @brief Parameters for the driver
 *
 * Parameters that should be provided by the application while registering the
 * driver. Mainly functions to set single pins.
 */
struct spi_gpio_params {
  /** Function for setting the clock pin */
  spi_gpio_set_pin_fn *set_clk;
  /** Application specific argument for the clock pin setter function */
  void *set_clk_arg;
  /** Function for setting the mosi pin */
  spi_gpio_set_pin_fn *set_mosi;
  /** Application specific argument for the mosi pin setter function */
  void *set_mosi_arg;
  /** Function for reading the miso pin */
  spi_gpio_get_pin_fn *get_miso;
  /** Application specific argument for the miso pin getter function */
  void *get_miso_arg;
  /** Functions for setting the cs pins */
  spi_gpio_set_pin_fn *set_cs[SPI_GPIO_MAX_CS];
  /** Application specific arguments for the cs pin getter functions */
  void *set_cs_arg[SPI_GPIO_MAX_CS];

  /**
   * Idle level of the CS pin.
   *
   * Set the value to 0 for a high active CS or to 1 for a low active CS pin.
   */
  bool cs_idle[SPI_GPIO_MAX_CS];
};

/**
 * Register a new SPI GPIO instance at the device path @a device.
 *
 * To save memory, @a params will be used directly. Make sure that the structure
 * remains valid during the complete application run time.
 *
 * @returns RTEMS_SUCCESSFUL if registering the bus worked or an error code if
 * it didn't.
 */
rtems_status_code spi_gpio_init(
    const char *device,
    const struct spi_gpio_params *params
  );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_DEV_SPI_SPI_GPIO_H */
