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

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H

#include <bsp/utility.h>

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

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_GPIO_H */
