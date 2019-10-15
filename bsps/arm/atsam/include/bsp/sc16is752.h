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

typedef struct {
  sc16is752_spi_context base;
  Pin irq_pin;
  rtems_interrupt_server_entry irqs_entry; /* Internal. Don't touch. */
  rtems_interrupt_server_action irqs_action; /* Internal. Don't touch. */
} atsam_sc16is752_spi_context;

/**
 * @brief Creates an SPI connected SC16IS752 device.
 *
 * This devices uses the interrupt server, see
 * rtems_interrupt_server_initialize().
 *
 * The device claims the interrupt of the PIO block.
 *
 * @param[in] ctx The device context.  May have an arbitrary content.
 * @param[in] device_path The device file path for the new device.
 * @param[in] mode The SC16IS752 mode.
 * @param[in] input_frequency The input frequency in Hertz of the SC16IS752
 *   chip.  See XTAL1 and XTAL2 pins.
 * @param[in] spi_path The SPI bus device path.
 * @param[in] spi_chip_select The SPI chip select (starts with 0, the SPI
 *   driver uses SPI_ChipSelect(1 << spi_chip_select)).
 * @param[in] spi_speed_hz The SPI bus speed in Hertz.
 * @param[in] irq_pin The interrupt pin, e.g. { PIO_PD28, PIOD, ID_PIOD,
 *   PIO_INPUT, PIO_IT_LOW_LEVEL }.
 *
 * @return See sc16is752_spi_create().
 */
int atsam_sc16is752_spi_create(
  atsam_sc16is752_spi_context *ctx,
  const char                  *device_path,
  sc16is752_mode               mode,
  uint32_t                     input_frequency,
  const char                  *spi_path,
  uint8_t                      spi_chip_select,
  uint32_t                     spi_speed_hz,
  const Pin                    *irq_pin
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_SC16IS752_H */
