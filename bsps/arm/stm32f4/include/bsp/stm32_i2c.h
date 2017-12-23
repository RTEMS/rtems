/**
 * @file
 * @ingroup stm32_i2c
 * @brief STM32 I2C support.
 */

/*
 * Copyright (c) 2013 Christian Mauderer.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32_I2C_H
#define LIBBSP_ARM_STM32F4_STM32_I2C_H

#include <bsp/utility.h>

/**
 * @defgroup stm32_i2c STM32 I2C Support
 * @ingroup stm32f4_i2c
 * @brief STM32 I2C Support
 * @{
 */

typedef struct {
	uint32_t cr1;
#define STM32F4_I2C_CR1_SWRST BSP_BIT32(15)
#define STM32F4_I2C_CR1_ALERT BSP_BIT32(13)
#define STM32F4_I2C_CR1_PEC BSP_BIT32(12)
#define STM32F4_I2C_CR1_POS BSP_BIT32(11)
#define STM32F4_I2C_CR1_ACK BSP_BIT32(10)
#define STM32F4_I2C_CR1_STOP BSP_BIT32(9)
#define STM32F4_I2C_CR1_START BSP_BIT32(8)
#define STM32F4_I2C_CR1_NOSTRETCH BSP_BIT32(7)
#define STM32F4_I2C_CR1_ENGC BSP_BIT32(6)
#define STM32F4_I2C_CR1_ENPEC BSP_BIT32(5)
#define STM32F4_I2C_CR1_ENARP BSP_BIT32(4)
#define STM32F4_I2C_CR1_SMBTYPE BSP_BIT32(3)
#define STM32F4_I2C_CR1_SMBUS BSP_BIT32(1)
#define STM32F4_I2C_CR1_PE BSP_BIT32(0)
	uint32_t cr2;
#define STM32F4_I2C_CR2_LAST BSP_BIT32(12)
#define STM32F4_I2C_CR2_DMAEN BSP_BIT32(11)
#define STM32F4_I2C_CR2_ITBUFEN BSP_BIT32(10)
#define STM32F4_I2C_CR2_ITEVTEN BSP_BIT32(9)
#define STM32F4_I2C_CR2_ITERREN BSP_BIT32(8)
#define STM32F4_I2C_CR2_FREQ(val) BSP_FLD32(val, 0, 5)
#define STM32F4_I2C_CR2_FREQ_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define STM32F4_I2C_CR2_FREQ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
	uint32_t oar1;
#define STM32F4_I2C_OAR1_ADDMODE BSP_BIT32(15)
#define STM32F4_I2C_OAR1_ADD(val) BSP_FLD32(val, 0, 9)
#define STM32F4_I2C_OAR1_ADD_GET(reg) BSP_FLD32GET(reg, 0, 9)
#define STM32F4_I2C_OAR1_ADD_SET(reg, val) BSP_FLD32SET(reg, val, 0, 9)
	uint32_t oar2;
#define STM32F4_I2C_OAR2_ADD2(val) BSP_FLD32(val, 1, 7)
#define STM32F4_I2C_OAR2_ADD2_GET(reg) BSP_FLD32GET(reg, 1, 7)
#define STM32F4_I2C_OAR2_ADD2_SET(reg, val) BSP_FLD32SET(reg, val, 1, 7)
#define STM32F4_I2C_OAR2_ENDUAL BSP_BIT32(0)
	uint32_t dr;
#define STM32F4_I2C_DR(val) BSP_FLD32(val, 0, 7)
#define STM32F4_I2C_DR_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define STM32F4_I2C_DR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t sr1;
#define STM32F4_I2C_SR1_SMBALERT BSP_BIT32(15)
#define STM32F4_I2C_SR1_TIMEOUT BSP_BIT32(14)
#define STM32F4_I2C_SR1_PECERR BSP_BIT32(12)
#define STM32F4_I2C_SR1_OVR BSP_BIT32(11)
#define STM32F4_I2C_SR1_AF BSP_BIT32(10)
#define STM32F4_I2C_SR1_ARLO BSP_BIT32(9)
#define STM32F4_I2C_SR1_BERR BSP_BIT32(8)
#define STM32F4_I2C_SR1_TxE BSP_BIT32(7)
#define STM32F4_I2C_SR1_RxNE BSP_BIT32(6)
#define STM32F4_I2C_SR1_STOPF BSP_BIT32(4)
#define STM32F4_I2C_SR1_ADD10 BSP_BIT32(3)
#define STM32F4_I2C_SR1_BTF BSP_BIT32(2)
#define STM32F4_I2C_SR1_ADDR BSP_BIT32(1)
#define STM32F4_I2C_SR1_SB BSP_BIT32(0)
	uint32_t sr2;
#define STM32F4_I2C_SR2_PEC(val) BSP_FLD32(val, 8, 15)
#define STM32F4_I2C_SR2_PEC_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define STM32F4_I2C_SR2_PEC_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define STM32F4_I2C_SR2_DUALF BSP_BIT32(7)
#define STM32F4_I2C_SR2_SMBHOST BSP_BIT32(6)
#define STM32F4_I2C_SR2_SMBDEFAULT BSP_BIT32(5)
#define STM32F4_I2C_SR2_GENCALL BSP_BIT32(4)
#define STM32F4_I2C_SR2_TRA BSP_BIT32(2)
#define STM32F4_I2C_SR2_BUSY BSP_BIT32(1)
#define STM32F4_I2C_SR2_MSL BSP_BIT32(0)
	uint32_t ccr;
#define STM32F4_I2C_CCR_FS BSP_BIT32(15)
#define STM32F4_I2C_CCR_DUTY BSP_BIT32(14)
#define STM32F4_I2C_CCR_CCR(val) BSP_FLD32(val, 0, 11)
#define STM32F4_I2C_CCR_CCR_GET(reg) BSP_FLD32GET(reg, 0, 11)
#define STM32F4_I2C_CCR_CCR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 11)
#define STM32F4_I2C_CCR_CCR_MAX STM32F4_I2C_CCR_CCR_GET(BSP_MSK32(0, 11))
	uint32_t trise;
#define STM32F4_I2C_TRISE(val) BSP_FLD32(val, 0, 5)
#define STM32F4_I2C_TRISE_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define STM32F4_I2C_TRISE_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
} stm32f4_i2c;

/** @} */

#endif /* LIBBSP_ARM_STM32F4_STM32_I2C_H */
