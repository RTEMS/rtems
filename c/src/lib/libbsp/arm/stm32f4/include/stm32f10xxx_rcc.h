/**
 * @file
 * @ingroup stm32f10xxx_rcc
 * @brief STM32F10XXX RCC support.
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

#ifndef LIBBSP_ARM_STM32F4_STM32F10XXX_RCC_H
#define LIBBSP_ARM_STM32F4_STM32F10XXX_RCC_H

#include <bsp/utility.h>

/**
 * @defgroup stm32f10xxx_rcc STM32F10XXX RCC Support
 * @ingroup stm32f4_rcc
 * @brief STM32F10XXX RCC Support
 * @{
 */

typedef struct {
	uint32_t cr;
	uint32_t cfgr;
	uint32_t cir;
	uint32_t apbrstr [2];
	uint32_t ahbenr [1];
	uint32_t apbenr [2];
	uint32_t bdcr;
	uint32_t csr;
	uint32_t ahbstr;
	uint32_t cfgr2;
} stm32f4_rcc;

/** @} */

#endif /* LIBBSP_ARM_STM32F4_STM32F10XXX_RCC_H */
