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

#ifndef LIBBSP_ARM_ATSAM_ATSAM_SPI_H
#define LIBBSP_ARM_ATSAM_ATSAM_SPI_H

#include <libchip/chip.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int spi_bus_register_atsam(
  const char *bus_path,
  uint8_t     spi_peripheral_id,
  Spi        *spi_regs,
  const Pin  *pins,
  size_t      pin_count
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_ATSAM_SPI_H */
