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

#ifndef LIBBSP_ARM_STM32F4_IO_H
#define LIBBSP_ARM_STM32F4_IO_H

#include <stdbool.h>

#include <bsp/stm32f4.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
  STM32F4_GPIO_MODE_INPUT,
  STM32F4_GPIO_MODE_OUTPUT,
  STM32F4_GPIO_MODE_AF,
  STM32F4_GPIO_MODE_ANALOG
} stm32f4_gpio_mode;

typedef enum {
  STM32F4_GPIO_OTYPE_PUSH_PULL,
  STM32F4_GPIO_OTYPE_OPEN_DRAIN
} stm32f4_gpio_otype;

typedef enum {
  STM32F4_GPIO_OSPEED_2_MHZ,
  STM32F4_GPIO_OSPEED_25_MHZ,
  STM32F4_GPIO_OSPEED_50_MHZ,
  STM32F4_GPIO_OSPEED_100_MHZ
} stm32f4_gpio_ospeed;

typedef enum {
  STM32F4_GPIO_NO_PULL,
  STM32F4_GPIO_PULL_UP,
  STM32F4_GPIO_PULL_DOWN
} stm32f4_gpio_pull;

typedef enum {
  STM32F4_GPIO_AF_SYSTEM = 0,
  STM32F4_GPIO_AF_TIM1 = 1,
  STM32F4_GPIO_AF_TIM2 = 1,
  STM32F4_GPIO_AF_TIM3 = 2,
  STM32F4_GPIO_AF_TIM4 = 2,
  STM32F4_GPIO_AF_TIM5 = 2,
  STM32F4_GPIO_AF_TIM8 = 3,
  STM32F4_GPIO_AF_TIM9 = 3,
  STM32F4_GPIO_AF_TIM10 = 3,
  STM32F4_GPIO_AF_TIM11 = 3,
  STM32F4_GPIO_AF_I2C1 = 4,
  STM32F4_GPIO_AF_I2C2 = 4,
  STM32F4_GPIO_AF_I2C3 = 4,
  STM32F4_GPIO_AF_SPI1 = 5,
  STM32F4_GPIO_AF_SPI2 = 5,
  STM32F4_GPIO_AF_SPI3 = 6,
  STM32F4_GPIO_AF_USART1 = 7,
  STM32F4_GPIO_AF_USART2 = 7,
  STM32F4_GPIO_AF_USART3 = 7,
  STM32F4_GPIO_AF_UART4 = 8,
  STM32F4_GPIO_AF_UART5 = 8,
  STM32F4_GPIO_AF_USART6 = 8,
  STM32F4_GPIO_AF_CAN1 = 9,
  STM32F4_GPIO_AF_CAN2 = 9,
  STM32F4_GPIO_AF_TIM12 = 9,
  STM32F4_GPIO_AF_TIM13 = 9,
  STM32F4_GPIO_AF_TIM14 = 9,
  STM32F4_GPIO_AF_OTG_FS = 10,
  STM32F4_GPIO_AF_OTG_HS = 10,
  STM32F4_GPIO_AF_ETH = 11,
  STM32F4_GPIO_AF_FSMC = 12,
  STM32F4_GPIO_AF_OTG_HS_FS = 12,
  STM32F4_GPIO_AF_SDIO = 12,
  STM32F4_GPIO_AF_DCMI = 13,
  STM32F4_GPIO_AF_EVENTOUT = 15
} stm32f4_gpio_af;

#define STM32F4_GPIO_PIN(port, index) ((((port) << 4) | (index)) & 0xff)

#define STM32F4_GPIO_PORT_OF_PIN(pin) (((pin) >> 4) & 0xf)

#define STM32F4_GPIO_INDEX_OF_PIN(pin) ((pin) & 0xf)

typedef struct {
  uint32_t pin : 8;
  uint32_t mode : 2;
  uint32_t otype : 1;
  uint32_t ospeed : 2;
  uint32_t pupd : 2;
  uint32_t af : 4;
} stm32f4_gpio_config;

void stm32f4_gpio_set_config(const stm32f4_gpio_config *config);

void stm32f4_gpio_set_output(int pin, bool set);

bool stm32f4_gpio_get_input(int pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_STM32F4_IO_H */
