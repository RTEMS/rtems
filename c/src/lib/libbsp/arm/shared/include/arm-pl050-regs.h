/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL050_REGS_H
#define LIBBSP_ARM_SHARED_ARM_PL050_REGS_H

#include <bsp/utility.h>

typedef struct {
	uint32_t kmicr;
#define PL050_KMICR_KMITYPE BSP_BIT32(5)
#define PL050_KMICR_KMIRXINTREN BSP_BIT32(4)
#define PL050_KMICR_KMITXINTREN BSP_BIT32(3)
#define PL050_KMICR_KMIEN BSP_BIT32(2)
#define PL050_KMICR_FKMID BSP_BIT32(1)
#define PL050_KMICR_FKMIC BSP_BIT32(0)
	uint32_t kmistat;
#define PL050_KMISTAT_TXEMPTY BSP_BIT32(6)
#define PL050_KMISTAT_TXBUSY BSP_BIT32(5)
#define PL050_KMISTAT_RXFULL BSP_BIT32(4)
#define PL050_KMISTAT_RXBUSY BSP_BIT32(3)
#define PL050_KMISTAT_RXPARITY BSP_BIT32(2)
#define PL050_KMISTAT_KMIC BSP_BIT32(1)
#define PL050_KMISTAT_KMID BSP_BIT32(0)
	uint32_t kmidata;
#define PL050_KMIDATA_KMIDATA(val) BSP_FLD32(val, 0, 7)
#define PL050_KMIDATA_KMIDATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define PL050_KMIDATA_KMIDATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t kmiclkdiv;
#define PL050_KMICLKDIV_KMICLKDIV(val) BSP_FLD32(val, 0, 3)
#define PL050_KMICLKDIV_KMICLKDIV_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define PL050_KMICLKDIV_KMICLKDIV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
	uint32_t kmiir;
#define PL050_KMIIR_KMITXINTR BSP_BIT32(1)
#define PL050_KMIIR_KMIRXINTR BSP_BIT32(0)
} pl050;

#endif /* LIBBSP_ARM_SHARED_ARM_PL050_REGS_H */
