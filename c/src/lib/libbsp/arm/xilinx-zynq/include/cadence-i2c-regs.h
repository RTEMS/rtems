/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_REGS_H
#define LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_REGS_H

#include <bsp/utility.h>

typedef struct {
	uint32_t control;
#define CADENCE_I2C_CONTROL_DIV_A(val) BSP_FLD32(val, 14, 15)
#define CADENCE_I2C_CONTROL_DIV_A_GET(reg) BSP_FLD32GET(reg, 14, 15)
#define CADENCE_I2C_CONTROL_DIV_A_SET(reg, val) BSP_FLD32SET(reg, val, 14, 15)
#define CADENCE_I2C_CONTROL_DIV_B(val) BSP_FLD32(val, 8, 13)
#define CADENCE_I2C_CONTROL_DIV_B_GET(reg) BSP_FLD32GET(reg, 8, 13)
#define CADENCE_I2C_CONTROL_DIV_B_SET(reg, val) BSP_FLD32SET(reg, val, 8, 13)
#define CADENCE_I2C_CONTROL_CLR_FIFO BSP_BIT32(6)
#define CADENCE_I2C_CONTROL_SLVMON BSP_BIT32(5)
#define CADENCE_I2C_CONTROL_HOLD BSP_BIT32(4)
#define CADENCE_I2C_CONTROL_ACKEN BSP_BIT32(3)
#define CADENCE_I2C_CONTROL_NEA BSP_BIT32(2)
#define CADENCE_I2C_CONTROL_MS BSP_BIT32(1)
#define CADENCE_I2C_CONTROL_RW BSP_BIT32(0)
	uint32_t status;
#define CADENCE_I2C_STATUS_BA BSP_BIT32(8)
#define CADENCE_I2C_STATUS_RXOVF BSP_BIT32(7)
#define CADENCE_I2C_STATUS_TXDV BSP_BIT32(6)
#define CADENCE_I2C_STATUS_RXDV BSP_BIT32(5)
#define CADENCE_I2C_STATUS_RXRW BSP_BIT32(3)
	uint32_t address;
#define CADENCE_I2C_ADDRESS(val) BSP_FLD32(val, 0, 9)
#define CADENCE_I2C_ADDRESS_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define CADENCE_I2C_ADDRESS_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
	uint32_t data;
	uint32_t irqstatus;
#define CADENCE_I2C_IXR_ARB_LOST BSP_BIT32(9)
#define CADENCE_I2C_IXR_RX_UNF BSP_BIT32(7)
#define CADENCE_I2C_IXR_TX_OVR BSP_BIT32(6)
#define CADENCE_I2C_IXR_RX_OVR BSP_BIT32(5)
#define CADENCE_I2C_IXR_SLV_RDY BSP_BIT32(4)
#define CADENCE_I2C_IXR_TO BSP_BIT32(3)
#define CADENCE_I2C_IXR_NACK BSP_BIT32(2)
#define CADENCE_I2C_IXR_DATA BSP_BIT32(1)
#define CADENCE_I2C_IXR_COMP BSP_BIT32(0)
	uint32_t transfer_size;
#define CADENCE_I2C_TRANSFER_SIZE(val) BSP_FLD32(val, 0, 7)
#define CADENCE_I2C_TRANSFER_SIZE_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define CADENCE_I2C_TRANSFER_SIZE_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t slave_mon_pause;
#define CADENCE_I2C_SLAVE_MON_PAUSE(val) BSP_FLD32(val, 0, 3)
#define CADENCE_I2C_SLAVE_MON_PAUSE_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define CADENCE_I2C_SLAVE_MON_PAUSE_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
	uint32_t timeout;
#define CADENCE_I2C_TIMEOUT(val) BSP_FLD32(val, 0, 7)
#define CADENCE_I2C_TIMEOUT_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define CADENCE_I2C_TIMEOUT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t irqmask;
	uint32_t irqenable;
	uint32_t irqdisable;
} cadence_i2c;

#endif /* LIBBSP_ARM_XILINX_ZYNQ_CADENCE_I2C_REGS_H */
