/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012 Sebastian Huber
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

#include <bsp/io.h>
#include <bsp.h>

const stm32f4_gpio_config stm32f4_start_config_gpio [] = {
#ifdef STM32F4_FAMILY_F4XXXX
  #ifdef STM32F4_ENABLE_USART_1
    STM32F4_PIN_USART1_TX_PA9,
    STM32F4_PIN_USART1_RX_PA10,
  #endif
  #ifdef STM32F4_ENABLE_USART_2
    STM32F4_PIN_USART2_TX_PA2,
    STM32F4_PIN_USART2_RX_PA3,
  #endif
  #ifdef STM32F4_ENABLE_USART_3
    STM32F4_PIN_USART3_TX_PD8,
    STM32F4_PIN_USART3_RX_PD9,
  #endif
  #ifdef STM32F4_ENABLE_UART_4
    STM32F4_PIN_UART4_TX_PA0,
    STM32F4_PIN_UART4_RX_PA1,
  #endif
  #ifdef STM32F4_ENABLE_UART_5
    STM32F4_PIN_UART5_TX_PC12,
    STM32F4_PIN_UART5_RX_PD2,
  #endif
  #ifdef STM32F4_ENABLE_USART_6
    STM32F4_PIN_USART6_TX_PC6,
    STM32F4_PIN_USART6_RX_PC7,
  #endif
  #ifdef STM32F4_ENABLE_I2C1
    #error Not implemented.
  #endif
  #ifdef STM32F4_ENABLE_I2C2
    #error Not implemented.
  #endif
#endif /* STM32F4_FAMILY_F4XXXX */
#ifdef STM32F4_FAMILY_F10XXX
  #ifdef STM32F4_ENABLE_USART_1
    STM32F4_PIN_USART1_TX_MAP_0,
    STM32F4_PIN_USART1_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_USART_2
    STM32F4_PIN_USART2_TX_MAP_0,
    STM32F4_PIN_USART2_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_USART_3
    STM32F4_PIN_USART3_TX_MAP_0,
    STM32F4_PIN_USART3_RX_MAP_0,
  #endif
  #ifdef STM32F4_ENABLE_UART_4
    STM32F4_PIN_UART4_TX,
    STM32F4_PIN_UART4_RX,
  #endif
  #ifdef STM32F4_ENABLE_UART_5
    STM32F4_PIN_UART5_TX,
    STM32F4_PIN_UART5_RX,
  #endif
  #ifdef STM32F4_ENABLE_USART_6
    #error STM32F10XXX has no USART 6
  #endif
  #ifdef STM32F4_ENABLE_I2C1
    STM32F4_PIN_I2C1_SCL_MAP0,
    STM32F4_PIN_I2C1_SDA_MAP0,
  #endif
  #ifdef STM32F4_ENABLE_I2C2
    STM32F4_PIN_I2C2_SCL,
    STM32F4_PIN_I2C2_SDA,
  #endif
#endif /* STM32F4_FAMILY_F10XXX */
  STM32F4_GPIO_CONFIG_TERMINAL
};
