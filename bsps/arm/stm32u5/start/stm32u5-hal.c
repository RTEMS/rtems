/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Implementation of support functions for the STM32U5 HAL
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stm32u5/hal.h>

#include <rtems.h>

stm32u5_module_index stm32u5_get_module_index( const void *regs )
{
  switch ( (uintptr_t) regs ) {
    case GPIOA_BASE:
      return STM32U5_MODULE_GPIOA;
    case GPIOB_BASE:
      return STM32U5_MODULE_GPIOB;
    case GPIOC_BASE:
      return STM32U5_MODULE_GPIOC;
    case GPIOD_BASE:
      return STM32U5_MODULE_GPIOD;
    case GPIOE_BASE:
      return STM32U5_MODULE_GPIOE;
    case GPIOF_BASE:
      return STM32U5_MODULE_GPIOF;
    case GPIOG_BASE:
      return STM32U5_MODULE_GPIOG;
    case GPIOH_BASE:
      return STM32U5_MODULE_GPIOH;
    case GPIOI_BASE:
      return STM32U5_MODULE_GPIOI;
    case GPIOJ_BASE:
      return STM32U5_MODULE_GPIOJ;
    case USART1_BASE:
      return STM32U5_MODULE_USART1;
    case USART2_BASE:
      return STM32U5_MODULE_USART2;
    case USART3_BASE:
      return STM32U5_MODULE_USART3;
    case UART4_BASE:
      return STM32U5_MODULE_UART4;
    case UART5_BASE:
      return STM32U5_MODULE_UART5;
    case USART6_BASE:
      return STM32U5_MODULE_USART6;
    case RNG_BASE:
      return STM32U5_MODULE_RNG;
    case SDMMC1_BASE:
    case DLYB_SDMMC1_BASE:
      return STM32U5_MODULE_SDMMC1;
    case SDMMC2_BASE:
    case DLYB_SDMMC2_BASE:
      return STM32U5_MODULE_SDMMC2;
  }

  return STM32U5_MODULE_INVALID;
}

typedef struct {
  __IO uint32_t *enr;
  uint32_t       enable_bit;
} stm32u5_clk_info;

static const stm32u5_clk_info stm32u5_clk[] = {
  [STM32U5_MODULE_INVALID] = { NULL, 0 },
  [STM32U5_MODULE_GPIOA]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOAEN },
  [STM32U5_MODULE_GPIOB]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOBEN },
  [STM32U5_MODULE_GPIOC]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOCEN },
  [STM32U5_MODULE_GPIOD]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIODEN },
  [STM32U5_MODULE_GPIOE]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOEEN },
  [STM32U5_MODULE_GPIOF]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOFEN },
  [STM32U5_MODULE_GPIOG]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOGEN },
  [STM32U5_MODULE_GPIOH]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOHEN },
  [STM32U5_MODULE_GPIOI]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOIEN },
  [STM32U5_MODULE_GPIOJ]   = { &RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOJEN },
  [STM32U5_MODULE_USART1]  = { &RCC->APB2ENR, RCC_APB2ENR_USART1EN },
  [STM32U5_MODULE_USART2]  = { &RCC->APB1ENR1, RCC_APB1ENR1_USART2EN },
  [STM32U5_MODULE_USART3]  = { &RCC->APB1ENR1, RCC_APB1ENR1_USART3EN },
  [STM32U5_MODULE_UART4]   = { &RCC->APB1ENR1, RCC_APB1ENR1_UART4EN },
  [STM32U5_MODULE_UART5]   = { &RCC->APB1ENR1, RCC_APB1ENR1_UART5EN },
  [STM32U5_MODULE_USART6]  = { &RCC->APB1ENR1, RCC_APB1ENR1_USART6EN },
  [STM32U5_MODULE_RNG]     = { &RCC->AHB2ENR1, RCC_AHB2ENR1_RNGEN },
  [STM32U5_MODULE_SDMMC1]  = { &RCC->AHB2ENR1, RCC_AHB2ENR1_SDMMC1EN },
  [STM32U5_MODULE_SDMMC2]  = { &RCC->AHB2ENR1, RCC_AHB2ENR1_SDMMC2EN },
};

void stm32u5_clk_enable( stm32u5_module_index index )
{
  __IO uint32_t        *enr;
  uint32_t              enable_bit;
  rtems_interrupt_level level;

  enr        = stm32u5_clk[ index ].enr;
  enable_bit = stm32u5_clk[ index ].enable_bit;

  rtems_interrupt_disable( level );
  SET_BIT( *enr, enable_bit );
  /* Delay after an RCC peripheral clock enabling */
  *enr;
  rtems_interrupt_enable( level );
}

void stm32u5_clk_disable( stm32u5_module_index index )
{
  __IO uint32_t        *enr;
  uint32_t              enable_bit;
  rtems_interrupt_level level;

  enr        = stm32u5_clk[ index ].enr;
  enable_bit = stm32u5_clk[ index ].enable_bit;

  rtems_interrupt_disable( level );
  CLEAR_BIT( *enr, enable_bit );
  rtems_interrupt_enable( level );
}

void stm32u5_gpio_init( const stm32u5_gpio_config *config )
{
  stm32u5_module_index index;

  index = stm32u5_get_module_index( config->regs );
  stm32u5_clk_enable( index );
  HAL_GPIO_Init( config->regs, &config->config );
}

void stm32u5_uart_polled_write( rtems_termios_device_context *base, char c )
{
  stm32u5_uart_context *ctx;
  USART_TypeDef        *regs;

  ctx  = stm32u5_uart_get_context( base );
  regs = ctx->uart.Instance;

  while ( ( regs->ISR & USART_ISR_TXE_TXFNF ) == 0 ) {
    /* Wait */
  }

  regs->TDR = (uint8_t) c;
}

int stm32u5_uart_polled_read( rtems_termios_device_context *base )
{
  stm32u5_uart_context *ctx;
  USART_TypeDef        *regs;

  ctx  = stm32u5_uart_get_context( base );
  regs = ctx->uart.Instance;

  if ( ( regs->ISR & USART_ISR_RXNE_RXFNE ) == 0 ) {
    return -1;
  }

  return (uint8_t) regs->RDR;
}
