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
	uint16_t iadr;
#define IMX_I2C_IADR_ADR(val) BSP_FLD16(val, 1, 7)
#define IMX_I2C_IADR_ADR_GET(reg) BSP_FLD16GET(reg, 1, 7)
#define IMX_I2C_IADR_ADR_SET(reg, val) BSP_FLD16SET(reg, val, 1, 7)
	uint16_t reserved_02;
	uint16_t ifdr;
#define IMX_I2C_IFDR_IC(val) BSP_FLD16(val, 0, 5)
#define IMX_I2C_IFDR_IC_GET(reg) BSP_FLD16GET(reg, 0, 5)
#define IMX_I2C_IFDR_IC_SET(reg, val) BSP_FLD16SET(reg, val, 0, 5)
	uint16_t reserved_06;
	uint16_t i2cr;
#define IMX_I2C_I2CR_IEN BSP_BIT16(7)
#define IMX_I2C_I2CR_IIEN BSP_BIT16(6)
#define IMX_I2C_I2CR_MSTA BSP_BIT16(5)
#define IMX_I2C_I2CR_MTX BSP_BIT16(4)
#define IMX_I2C_I2CR_TXAK BSP_BIT16(3)
#define IMX_I2C_I2CR_RSTA BSP_BIT16(2)
	uint16_t reserved_0a;
	uint16_t i2sr;
#define IMX_I2C_I2SR_ICF BSP_BIT16(7)
#define IMX_I2C_I2SR_IAAS BSP_BIT16(6)
#define IMX_I2C_I2SR_IBB BSP_BIT16(5)
#define IMX_I2C_I2SR_IAL BSP_BIT16(4)
#define IMX_I2C_I2SR_SRW BSP_BIT16(2)
#define IMX_I2C_I2SR_IIF BSP_BIT16(1)
#define IMX_I2C_I2SR_RXAK BSP_BIT16(0)
	uint16_t reserved_0e;
	uint16_t i2dr;
#define IMX_I2C_I2DR_DATA(val) BSP_FLD16(val, 0, 7)
#define IMX_I2C_I2DR_DATA_GET(reg) BSP_FLD16GET(reg, 0, 7)
#define IMX_I2C_I2DR_DATA_SET(reg, val) BSP_FLD16SET(reg, val, 0, 7)
} imx_i2c;

#endif /* IMX_I2CREG_H */
