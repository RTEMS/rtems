/**
 * @file
 * @ingroup stm32f4_exti
 * @brief STM32F10XXX EXTI support
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

#ifndef LIBBSP_ARM_STM32F4_STM32F10XXX_EXTI_H
#define LIBBSP_ARM_STM32F4_STM32F10XXX_EXTI_H

#include <bsp/utility.h>

/**
 * @defgroup stm32f4_exti EXTI Support
 * @ingroup RTEMSBSPsARMSTM32F4
 * @brief STM32F10XXX EXTI Support
 * @{
 */

typedef struct {
	uint32_t imr;
#define STM32F4_EXTI_IMR(line) BSP_BIT32(line)
	uint32_t emr;
#define STM32F4_EXTI_EMR(line) BSP_BIT32(line)
	uint32_t rtsr;
#define STM32F4_EXTI_RTSR(line) BSP_BIT32(line)
	uint32_t ftsr;
#define STM32F4_EXTI_FTSR(line) BSP_BIT32(line)
	uint32_t swier;
#define STM32F4_EXTI_SWIER(line) BSP_BIT32(line)
	uint32_t pr;
#define STM32F4_EXTI_PR(line) BSP_BIT32(line)
} stm32f4_exti;

/** @} */

#endif /* LIBBSP_ARM_STM32F4_STM32F10XXX_EXTI_H */
