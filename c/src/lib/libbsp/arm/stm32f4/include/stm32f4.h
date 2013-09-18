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
#include <bspopts.h>

#define STM32F4_BASE 0x00

#ifdef STM32F4_FAMILY_F4XXXX

#include <bsp/stm32f4xxxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40020000) + (i))

#include <bsp/stm32f4xxxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40023800))

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011000))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))
#define STM32F4_USART_6 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40011400))

#endif /* STM32F4_FAMILY_F4XXXX */

#ifdef STM32F4_FAMILY_F10XXX

#include <bsp/stm32f10xxx_gpio.h>
#define STM32F4_GPIO(i) ((volatile stm32f4_gpio *) (STM32F4_BASE + 0x40010800 + i * 0x400))
#define STM32F4_AFIO ((volatile stm32f4_afio *) (STM32F4_BASE + 0x40010000))

#include <bsp/stm32f10xxx_rcc.h>
#define STM32F4_RCC ((volatile stm32f4_rcc *) (STM32F4_BASE + 0x40021000))

#include <bsp/stm32_usart.h>
#define STM32F4_USART_1 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40013800))
#define STM32F4_USART_2 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004400))
#define STM32F4_USART_3 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004800))
#define STM32F4_USART_4 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40004c00))
#define STM32F4_USART_5 ((volatile stm32f4_usart *) (STM32F4_BASE + 0x40005000))

#endif /* STM32F4_FAMILY_F10XXX */

#endif /* LIBBSP_ARM_STM32F4_STM32F4_H */
