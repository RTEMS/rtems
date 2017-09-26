/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef IMX_I2CREG_H
#define IMX_I2CREG_H

#include <bsp/utility.h>

typedef struct {
	uint32_t iadr;
#define IMX_I2C_IADR_ADR(val) BSP_FLD32(val, 1, 7)
#define IMX_I2C_IADR_ADR_GET(reg) BSP_FLD32GET(reg, 1, 7)
#define IMX_I2C_IADR_ADR_SET(reg, val) BSP_FLD32SET(reg, val, 1, 7)
	uint32_t ifdr;
#define IMX_I2C_IFDR_IC(val) BSP_FLD32(val, 0, 5)
#define IMX_I2C_IFDR_IC_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define IMX_I2C_IFDR_IC_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
	uint32_t i2cr;
#define IMX_I2C_I2CR_IEN BSP_BIT32(7)
#define IMX_I2C_I2CR_IIEN BSP_BIT32(6)
#define IMX_I2C_I2CR_MSTA BSP_BIT32(5)
#define IMX_I2C_I2CR_MTX BSP_BIT32(4)
#define IMX_I2C_I2CR_TXAK BSP_BIT32(3)
#define IMX_I2C_I2CR_RSTA BSP_BIT32(2)
	uint32_t i2sr;
#define IMX_I2C_I2SR_ICF BSP_BIT32(7)
#define IMX_I2C_I2SR_IAAS BSP_BIT32(6)
#define IMX_I2C_I2SR_IBB BSP_BIT32(5)
#define IMX_I2C_I2SR_IAL BSP_BIT32(4)
#define IMX_I2C_I2SR_SRW BSP_BIT32(2)
#define IMX_I2C_I2SR_IIF BSP_BIT32(1)
#define IMX_I2C_I2SR_RXAK BSP_BIT32(0)
	uint32_t i2dr;
#define IMX_I2C_I2DR_DATA(val) BSP_FLD32(val, 0, 7)
#define IMX_I2C_I2DR_DATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define IMX_I2C_I2DR_DATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
} imx_i2c;

#endif /* IMX_I2CREG_H */
