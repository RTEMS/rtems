/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_SC16IS752_H
#define LIBBSP_ARM_ATSAM_SC16IS752_H

#include <rtems/irq-extension.h>
#include <libchip/chip.h>

#include <dev/serial/sc16is752.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The SC16IS752 device context.
 *
 * All members are private to the device driver.
 */
typedef struct {
  sc16is752_spi_context base;
  Pin irq_pin;
  rtems_interrupt_server_entry irqs_entry;
  rtems_interrupt_server_action irqs_action;
  uint32_t irqs_index;
} atsam_sc16is752_spi_context;

/**
 * @brief The SC16IS752 device configuration.
 *
 * @see atsam_sc16is752_spi_create().
 */
typedef struct {
  /**
   * @brief The device file path for the new device.
   */
  const char *device_path;

  /**
   * @brief The SC16IS752 mode.
   */
  sc16is752_mode mode;

  /**
   * @brief The input frequency in Hertz of the SC16IS752 chip.  See XTAL1 and
   * XTAL2 pins.
   */
  uint32_t input_frequency;

  /**
   * @brief The SPI bus device path.
   */
  const char *spi_path;

  /**
   * @brief The SPI chip select (starts with 0, the SPI driver uses
   * SPI_ChipSelect(1 << spi_chip_select)).
   */
  uint8_t spi_chip_select;

  /**
   * @brief The SPI bus speed in Hertz.
   */
  uint32_t spi_speed_hz;

  /**
   * @brief The interrupt pin, e.g. { PIO_PD28, PIOD, ID_PIOD, PIO_INPUT,
   * PIO_IT_LOW_LEVEL }.
   */
  const Pin irq_pin;

  /**
   * @brief The index to identify the interrupt server used for interrupt
   * processing.
   */
  uint32_t server_index;
} atsam_sc16is752_spi_config;

/**
 * @brief Creates an SPI connected SC16IS752 device.
 *
 * This devices uses the interrupt server, see rtems_interrupt_server_create().
 *
 * The device claims the interrupt of the PIO block.
 *
 * @param[out] ctx is the device context.  It may have an arbitrary content.
 * @param config is the device configuration.
 *
 * @return See sc16is752_spi_create().
 */
int atsam_sc16is752_spi_create(
  atsam_sc16is752_spi_context      *ctx,
  const atsam_sc16is752_spi_config *config
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_SC16IS752_H */
