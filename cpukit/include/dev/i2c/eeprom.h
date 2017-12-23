/**
 * @file
 *
 * @brief EEPROM Driver API
 *
 * @ingroup I2CEEPROM
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _DEV_I2C_EEPROM_H
#define _DEV_I2C_EEPROM_H

#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup I2CEEPROM EEPROM Driver
 *
 * @ingroup I2CDevice
 *
 * @brief Driver for EEPROM device.
 *
 * @{
 */

int i2c_dev_register_eeprom(
  const char *bus_path,
  const char *dev_path,
  uint16_t i2c_address,
  uint16_t address_bytes,
  uint16_t page_size_in_bytes,
  uint32_t size_in_bytes,
  uint32_t program_timeout_in_ms
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DEV_I2C_EEPROM_H */
