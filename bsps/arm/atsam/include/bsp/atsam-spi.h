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

typedef struct {
  uint8_t     spi_peripheral_id;
  const Pin  *pins;
  Spi        *spi_regs;
  size_t      pin_count;
  bool        chip_select_decode;

  /* Delay before SCLK in ns */
  uint32_t    dlybs_in_ns[4];

  /* Delay before consecutive word transfers in ns */
  uint32_t    dlybct_in_ns[4];
} atsam_spi_config;

int spi_bus_register_atsam(
  const char *bus_path,
  const atsam_spi_config *config
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_ATSAM_SPI_H */
