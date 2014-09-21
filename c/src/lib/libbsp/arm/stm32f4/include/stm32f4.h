/**
 * @file
 * @ingroup stm32f4_reg
 * @brief Register definitions.
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4_H
#define LIBBSP_ARM_STM32F4_STM32F4_H

#include <bsp/utility.h>
#include <bspopts.h>

#define STM32F4_BASE 0x00

#ifdef STM32F4_FAMILY_F4XXXX

/**
 * @defgroup stm32f4_reg Register Defintions
 * @ingroup arm_stm32f4
 * @brief Register Definitions
 * @{
 */

/**
 * @name STM32f4XXXX GPIO
 * @{
 */

#include <bsp/stm32f4xxxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40020000) + (i))

/** @} */

/**
 * @name STM32F4XXXX RCC
 * @{
 */

#include <bsp/stm32f4xxxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40023800))

/** @} */

/**
 * @name STM32F4XXXX FLASH
 * @{
 */

#include <bsp/stm32f4xxxx_flash.h>
#define STM32F4_FLASH ((volatile stm32f4_flash *) (STM32F4_BASE + 0x40023C00))

/** @} */

#include <bsp/stm32_i2c.h>

/**
 * @name STM32 I2C
 * @{
 */

#define STM32F4_I2C3 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005C00))
#define STM32F4_I2C2 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005800))
#define STM32F4_I2C1 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005400))

/** @} */

/**
 * @name STM32 USART
 * @{
 */

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011000))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))
#define STM32F4_USART_6 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011400))

/** @} */

#endif /* STM32F4_FAMILY_F4XXXX */

#ifdef STM32F4_FAMILY_F10XXX

/**
 * @name STM32F10 EXTI
 * @{
 */

#include <bsp/stm32f10xxx_exti.h>
#define STM32F4_EXTI ((volatile stm32f4_exti *) (STM32F4_BASE + 0x40010400))

/** @} */

/**
 * @name STM32F10XXX GPIO
 * @{
 */

#include <bsp/stm32f10xxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40010800 + i * 0x400))
#define STM32F4_AFIO ((volatile stm32f4_afio *) (STM32F4_BASE + 0x40010000))

/** @} */

/**
 * @name STM32F10XXX RCC
 * @{
 */

#include <bsp/stm32f10xxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40021000))

/** @} */

/**
 * @name STM32 I2C
 * @{
 */

#include <bsp/stm32_i2c.h>
#define STM32F4_I2C2 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005800))
#define STM32F4_I2C1 ((volatile stm32f4_i2c *) (STM32F4_BASE + 0x40005400))

/** @} */

/**
 * @name STM32 USART
 * @{
 */

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40013800))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))

/** @} */

/** @} */

#endif /* STM32F4_FAMILY_F10XXX */

#endif /* LIBBSP_ARM_STM32F4_STM32F4_H */
