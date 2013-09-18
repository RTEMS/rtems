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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H

#include <bsp/utility.h>

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

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_RCC_H */
