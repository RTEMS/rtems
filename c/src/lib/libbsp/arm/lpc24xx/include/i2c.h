/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the I2C modules.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_I2C_H
#define LIBBSP_ARM_LPC24XX_I2C_H

#include <rtems.h>
#include <rtems/libi2c.h>

#include <bsp/io.h>
#include <bsp/lpc24xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc24xx_libi2c LPC24XX Bus Drivers
 *
 * @ingroup libi2c
 *
 * @brief LibI2C bus drivers for LPC24XX.
 *
 * @{
 */

typedef struct {
  rtems_libi2c_bus_t bus;
  volatile lpc24xx_i2c *regs;
  size_t index;
  const lpc24xx_pin_range *pins;
  rtems_vector_number vector;
  rtems_id state_update;
  uint8_t *volatile data;
  uint8_t *volatile end;
} lpc24xx_i2c_bus_entry;

extern const rtems_libi2c_bus_ops_t lpc24xx_i2c_ops;

extern rtems_libi2c_bus_t *const lpc24xx_i2c_0;

extern rtems_libi2c_bus_t *const lpc24xx_i2c_1;

extern rtems_libi2c_bus_t *const lpc24xx_i2c_2;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_I2C_H */
