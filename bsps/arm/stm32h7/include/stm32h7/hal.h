/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_ARM_STM32H7_STM32H7_HAL_H
#define LIBBSP_ARM_STM32H7_STM32H7_HAL_H

#include <stm32h7xx_hal.h>

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  STM32H7_MODULE_INVALID,
  STM32H7_MODULE_GPIOA,
  STM32H7_MODULE_GPIOB,
  STM32H7_MODULE_GPIOC,
  STM32H7_MODULE_GPIOD,
  STM32H7_MODULE_GPIOE,
  STM32H7_MODULE_GPIOF,
  STM32H7_MODULE_GPIOG,
  STM32H7_MODULE_GPIOH,
  STM32H7_MODULE_GPIOI,
  STM32H7_MODULE_GPIOJ,
  STM32H7_MODULE_GPIOK,
  STM32H7_MODULE_USART1,
  STM32H7_MODULE_USART2,
  STM32H7_MODULE_USART3,
  STM32H7_MODULE_UART4,
  STM32H7_MODULE_UART5,
  STM32H7_MODULE_USART6,
  STM32H7_MODULE_UART7,
  STM32H7_MODULE_UART8,
  STM32H7_MODULE_UART9,
  STM32H7_MODULE_USART10,
  STM32H7_MODULE_RNG,
  STM32H7_MODULE_ETH1MAC,
  STM32H7_MODULE_ETH1TX,
  STM32H7_MODULE_ETH1RX,
  STM32H7_MODULE_USB1_OTG,
  STM32H7_MODULE_USB1_OTG_ULPI,
  STM32H7_MODULE_USB2_OTG,
  STM32H7_MODULE_USB2_OTG_ULPI,
  STM32H7_MODULE_SDMMC1,
  STM32H7_MODULE_SDMMC2,
} stm32h7_module_index;

stm32h7_module_index stm32h7_get_module_index(const void *regs);

void stm32h7_clk_enable(stm32h7_module_index index);

void stm32h7_clk_disable(stm32h7_module_index index);

void stm32h7_clk_low_power_enable(stm32h7_module_index index);

void stm32h7_clk_low_power_disable(stm32h7_module_index index);

typedef struct {
  GPIO_TypeDef *regs;
  GPIO_InitTypeDef config;
} stm32h7_gpio_config;

void stm32h7_gpio_init(const stm32h7_gpio_config *config);

typedef struct {
  stm32h7_gpio_config gpio;
  rtems_vector_number irq;
  uint8_t device_index;
} stm32h7_uart_config;

typedef struct {
  UART_HandleTypeDef uart;
  bool transmitting;
  rtems_termios_device_context device;
  const stm32h7_uart_config *config;
} stm32h7_uart_context;

static inline stm32h7_uart_context *stm32h7_uart_get_context(
  rtems_termios_device_context *base
)
{
  return RTEMS_CONTAINER_OF(base, stm32h7_uart_context, device);
}

void stm32h7_uart_polled_write(rtems_termios_device_context *base, char c);

int stm32h7_uart_polled_read(rtems_termios_device_context *base);

extern stm32h7_uart_context stm32h7_usart1_instance;

extern const stm32h7_uart_config stm32h7_usart1_config;

extern stm32h7_uart_context stm32h7_usart2_instance;

extern const stm32h7_uart_config stm32h7_usart2_config;

extern stm32h7_uart_context stm32h7_usart3_instance;

extern const stm32h7_uart_config stm32h7_usart3_config;

extern stm32h7_uart_context stm32h7_uart4_instance;

extern const stm32h7_uart_config stm32h7_uart4_config;

extern stm32h7_uart_context stm32h7_uart5_instance;

extern const stm32h7_uart_config stm32h7_uart5_config;

extern stm32h7_uart_context stm32h7_usart6_instance;

extern const stm32h7_uart_config stm32h7_usart6_config;

extern stm32h7_uart_context stm32h7_uart7_instance;

extern const stm32h7_uart_config stm32h7_uart7_config;

extern stm32h7_uart_context stm32h7_uart8_instance;

extern const stm32h7_uart_config stm32h7_uart8_config;

extern stm32h7_uart_context stm32h7_uart9_instance;

extern const stm32h7_uart_config stm32h7_uart9_config;

extern stm32h7_uart_context stm32h7_usart10_instance;

extern const stm32h7_uart_config stm32h7_usart10_config;

extern const uint32_t stm32h7_config_pwr_regulator_voltagescaling;

extern const RCC_OscInitTypeDef stm32h7_config_oscillator;

extern const RCC_ClkInitTypeDef stm32h7_config_clocks;

extern const uint32_t stm32h7_config_flash_latency;

extern const RCC_PeriphCLKInitTypeDef stm32h7_config_peripheral_clocks;

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_STM32H7_STM32H7_HAL_H */
