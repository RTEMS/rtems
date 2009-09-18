/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the I2C modules.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_I2C_H
#define LIBBSP_ARM_LPC24XX_I2C_H

#include <rtems/libi2c.h>

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

extern rtems_libi2c_bus_t * const lpc24xx_i2c_0;

extern rtems_libi2c_bus_t * const lpc24xx_i2c_1;

extern rtems_libi2c_bus_t * const lpc24xx_i2c_2;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_I2C_H */
