/**
 * @file
 *
 * @ingroup lpc24xx_libi2c
 *
 * @brief LibI2C bus driver for the Synchronous Serial Port (SSP).
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_SSP_H
#define LIBBSP_ARM_LPC24XX_SSP_H

#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup lpc24xx_libi2c
 *
 * @{
 */

extern rtems_libi2c_bus_t * const lpc24xx_ssp_0;

extern rtems_libi2c_bus_t * const lpc24xx_ssp_1;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_SSP_H */
