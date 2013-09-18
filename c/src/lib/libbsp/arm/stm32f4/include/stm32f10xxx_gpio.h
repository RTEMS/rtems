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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F10XXX_GPIO_H
#define LIBBSP_ARM_STM32F4_STM32F10XXX_GPIO_H

#include <bsp/utility.h>

typedef struct {
	uint32_t cr[2];
	uint32_t idr;
	uint32_t odr;
	uint32_t bsrr;
	uint32_t brr;
	uint32_t lckr;
} stm32f4_gpio;

typedef struct {
	uint32_t evcr;
	uint32_t mapr;
	uint32_t exticr[4];
	uint32_t mapr2;
} stm32f4_afio;

#endif /* LIBBSP_ARM_STM32F4_STM32F10XXX_GPIO_H */
