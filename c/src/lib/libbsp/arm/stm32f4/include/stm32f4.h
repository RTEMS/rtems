/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

#ifndef LIBBSP_ARM_STM32F4_STM32F4_H
#define LIBBSP_ARM_STM32F4_STM32F4_H

#include <bsp/utility.h>

#define STM32F4_BASE 0x00

typedef struct {
	uint32_t moder;
	uint32_t otyper;
	uint32_t ospeedr;
	uint32_t pupdr;
	uint32_t idr;
	uint32_t odr;
	uint32_t bsrr;
	uint32_t lckr;
	uint32_t afr [2];
	uint32_t reserved_28 [246];
} stm32f4_gpio;

#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40020000) + (i))

typedef struct {
	uint32_t cr;
	uint32_t pllcfgr;
	uint32_t cfgr;
	uint32_t cir;
	uint32_t ahbrstr [3];
	uint32_t reserved_1c;
	uint32_t apbrstr [2];
	uint32_t reserved_28 [2];
	uint32_t ahbenr [3];
	uint32_t reserved_3c;
	uint32_t apbenr [2];
	uint32_t reserved_48 [2];
	uint32_t ahblpenr [3];
	uint32_t reserved_5c;
	uint32_t apblpenr [2];
	uint32_t reserved_68 [2];
	uint32_t bdcr;
	uint32_t csr;
	uint32_t reserved_78 [2];
	uint32_t sscgr;
	uint32_t plli2scfgr;
} stm32f4_rcc;

#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40023800))

typedef struct {
	uint32_t sr;
#define STM32F4_USART_SR_CTS BSP_BIT32(9)
#define STM32F4_USART_SR_LBD BSP_BIT32(8)
#define STM32F4_USART_SR_TXE BSP_BIT32(7)
#define STM32F4_USART_SR_TC BSP_BIT32(6)
#define STM32F4_USART_SR_RXNE BSP_BIT32(5)
#define STM32F4_USART_SR_IDLE BSP_BIT32(4)
#define STM32F4_USART_SR_ORE BSP_BIT32(3)
#define STM32F4_USART_SR_NF BSP_BIT32(2)
#define STM32F4_USART_SR_FE BSP_BIT32(1)
#define STM32F4_USART_SR_PE BSP_BIT32(0)
	uint32_t dr;
#define STM32F4_USART_DR(val) BSP_FLD32(val, 0, 7)
#define STM32F4_USART_DR_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define STM32F4_USART_DR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
	uint32_t bbr;
#define STM32F4_USART_BBR_DIV_MANTISSA(val) BSP_FLD32(val, 4, 15)
#define STM32F4_USART_BBR_DIV_MANTISSA_GET(reg) BSP_FLD32GET(reg, 4, 15)
#define STM32F4_USART_BBR_DIV_MANTISSA_SET(reg, val) BSP_FLD32SET(reg, val, 4, 15)
#define STM32F4_USART_BBR_DIV_FRACTION(val) BSP_FLD32(val, 0, 3)
#define STM32F4_USART_BBR_DIV_FRACTION_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define STM32F4_USART_BBR_DIV_FRACTION_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
	uint32_t cr1;
#define STM32F4_USART_CR1_OVER8 BSP_BIT32(15)
#define STM32F4_USART_CR1_UE BSP_BIT32(13)
#define STM32F4_USART_CR1_M BSP_BIT32(12)
#define STM32F4_USART_CR1_WAKE BSP_BIT32(11)
#define STM32F4_USART_CR1_PCE BSP_BIT32(10)
#define STM32F4_USART_CR1_PS BSP_BIT32(9)
#define STM32F4_USART_CR1_PEIE BSP_BIT32(8)
#define STM32F4_USART_CR1_TXEIE BSP_BIT32(7)
#define STM32F4_USART_CR1_TCIE BSP_BIT32(6)
#define STM32F4_USART_CR1_RXNEIE BSP_BIT32(5)
#define STM32F4_USART_CR1_IDLEIE BSP_BIT32(4)
#define STM32F4_USART_CR1_TE BSP_BIT32(3)
#define STM32F4_USART_CR1_RE BSP_BIT32(2)
#define STM32F4_USART_CR1_RWU BSP_BIT32(1)
#define STM32F4_USART_CR1_SBK BSP_BIT32(0)
	uint32_t cr2;
#define STM32F4_USART_CR2_LINEN BSP_BIT32(14)
#define STM32F4_USART_CR2_STOP(val) BSP_FLD32(val, 12, 13)
#define STM32F4_USART_CR2_STOP_GET(reg) BSP_FLD32GET(reg, 12, 13)
#define STM32F4_USART_CR2_STOP_SET(reg, val) BSP_FLD32SET(reg, val, 12, 13)
#define STM32F4_USART_CR2_CLKEN BSP_BIT32(11)
#define STM32F4_USART_CR2_CPOL BSP_BIT32(10)
#define STM32F4_USART_CR2_CPHA BSP_BIT32(9)
#define STM32F4_USART_CR2_LBCL BSP_BIT32(8)
#define STM32F4_USART_CR2_LBDIE BSP_BIT32(6)
#define STM32F4_USART_CR2_LBDL BSP_BIT32(5)
#define STM32F4_USART_CR2_ADD(val) BSP_FLD32(val, 0, 3)
#define STM32F4_USART_CR2_ADD_GET(reg) BSP_FLD32GET(reg, 0, 3)
#define STM32F4_USART_CR2_ADD_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
	uint32_t cr3;
#define STM32F4_USART_CR3_ONEBIT BSP_BIT32(11)
#define STM32F4_USART_CR3_CTSIE BSP_BIT32(10)
#define STM32F4_USART_CR3_CTSE BSP_BIT32(9)
#define STM32F4_USART_CR3_RTSE BSP_BIT32(8)
#define STM32F4_USART_CR3_DMAT BSP_BIT32(7)
#define STM32F4_USART_CR3_DMAR BSP_BIT32(6)
#define STM32F4_USART_CR3_SCEN BSP_BIT32(5)
#define STM32F4_USART_CR3_NACK BSP_BIT32(4)
#define STM32F4_USART_CR3_HDSEL BSP_BIT32(3)
#define STM32F4_USART_CR3_IRLP BSP_BIT32(2)
#define STM32F4_USART_CR3_IREN BSP_BIT32(1)
#define STM32F4_USART_CR3_EIE BSP_BIT32(0)
	uint32_t gtpr;
#define STM32F4_USART_GTPR_GT(val) BSP_FLD32(val, 8, 15)
#define STM32F4_USART_GTPR_GT_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define STM32F4_USART_GTPR_GT_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define STM32F4_USART_GTPR_PSC(val) BSP_FLD32(val, 0, 7)
#define STM32F4_USART_GTPR_PSC_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define STM32F4_USART_GTPR_PSC_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
} stm32f4_usart;

#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011000))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))
#define STM32F4_USART_6 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011400))

typedef struct {
	uint32_t reserved_00 [268439808];
	stm32f4_usart usart_2;
	uint32_t reserved_4000441c [249];
	stm32f4_usart usart_3;
	uint32_t reserved_4000481c [249];
	stm32f4_usart usart_4;
	uint32_t reserved_40004c1c [249];
	stm32f4_usart usart_5;
	uint32_t reserved_4000501c [12281];
	stm32f4_usart usart_1;
	uint32_t reserved_4001101c [249];
	stm32f4_usart usart_6;
	uint32_t reserved_4001141c [15097];
	stm32f4_gpio gpio [9];
	uint32_t reserved_40022400 [1280];
	stm32f4_rcc rcc;
} stm32f4;

#define STM32F4 (*(volatile stm32f4 *) (STM32F4_BASE))

#endif /* LIBBSP_ARM_STM32F4_STM32F4_H */
