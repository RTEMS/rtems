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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stm32h7/hal.h>

#include <rtems.h>

stm32h7_module_index stm32h7_get_module_index(const void *regs)
{
  switch ((uintptr_t) regs) {
    case GPIOA_BASE:
      return STM32H7_MODULE_GPIOA;
    case GPIOB_BASE:
      return STM32H7_MODULE_GPIOB;
    case GPIOC_BASE:
      return STM32H7_MODULE_GPIOC;
    case GPIOD_BASE:
      return STM32H7_MODULE_GPIOD;
    case GPIOE_BASE:
      return STM32H7_MODULE_GPIOE;
    case GPIOF_BASE:
      return STM32H7_MODULE_GPIOF;
    case GPIOG_BASE:
      return STM32H7_MODULE_GPIOG;
    case GPIOH_BASE:
      return STM32H7_MODULE_GPIOH;
    case GPIOI_BASE:
      return STM32H7_MODULE_GPIOI;
    case GPIOJ_BASE:
      return STM32H7_MODULE_GPIOJ;
    case GPIOK_BASE:
      return STM32H7_MODULE_GPIOK;
    case USART1_BASE:
      return STM32H7_MODULE_USART1;
    case USART2_BASE:
      return STM32H7_MODULE_USART2;
    case USART3_BASE:
      return STM32H7_MODULE_USART3;
    case UART4_BASE:
      return STM32H7_MODULE_UART4;
    case UART5_BASE:
      return STM32H7_MODULE_UART5;
    case USART6_BASE:
      return STM32H7_MODULE_USART6;
    case UART7_BASE:
      return STM32H7_MODULE_UART7;
    case UART8_BASE:
      return STM32H7_MODULE_UART8;
#ifdef UART9_BASE
    case UART9_BASE:
      return STM32H7_MODULE_UART9;
#endif
#ifdef USART10_BASE
    case USART10_BASE:
      return STM32H7_MODULE_USART10;
#endif
    case RNG_BASE:
      return STM32H7_MODULE_RNG;
    case SDMMC1_BASE:
    case DLYB_SDMMC1_BASE:
      return STM32H7_MODULE_SDMMC1;
    case SDMMC2_BASE:
    case DLYB_SDMMC2_BASE:
      return STM32H7_MODULE_SDMMC2;
  }

  return STM32H7_MODULE_INVALID;
}

typedef struct {
  __IO uint32_t *enr;
  uint32_t enable_bit;
} stm32h7_clk_info;

static const stm32h7_clk_info stm32h7_clk[] = {
  [STM32H7_MODULE_INVALID] = { NULL, 0 },
  [STM32H7_MODULE_GPIOA] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOAEN },
  [STM32H7_MODULE_GPIOB] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOBEN },
  [STM32H7_MODULE_GPIOC] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOCEN },
  [STM32H7_MODULE_GPIOD] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIODEN },
  [STM32H7_MODULE_GPIOE] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN },
  [STM32H7_MODULE_GPIOF] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOFEN },
  [STM32H7_MODULE_GPIOG] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOGEN },
  [STM32H7_MODULE_GPIOH] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOHEN },
  [STM32H7_MODULE_GPIOI] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOIEN },
  [STM32H7_MODULE_GPIOJ] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOJEN },
  [STM32H7_MODULE_GPIOK] = { &RCC->AHB4ENR, RCC_AHB4ENR_GPIOKEN },
  [STM32H7_MODULE_USART1] = { &RCC->APB2ENR, RCC_APB2ENR_USART1EN },
  [STM32H7_MODULE_USART2] = { &RCC->APB1LENR, RCC_APB1LENR_USART2EN },
  [STM32H7_MODULE_USART3] = { &RCC->APB1LENR, RCC_APB1LENR_USART3EN },
  [STM32H7_MODULE_UART4] = { &RCC->APB1LENR, RCC_APB1LENR_UART4EN },
  [STM32H7_MODULE_UART5] = { &RCC->APB1LENR, RCC_APB1LENR_UART5EN },
  [STM32H7_MODULE_USART6] = { &RCC->APB2ENR, RCC_APB2ENR_USART6EN },
  [STM32H7_MODULE_UART7] = { &RCC->APB1LENR, RCC_APB1LENR_UART7EN },
  [STM32H7_MODULE_UART8] = { &RCC->APB1LENR, RCC_APB1LENR_UART8EN },
#ifdef UART9_BASE
  [STM32H7_MODULE_UART9] = { &RCC->APB2ENR, RCC_APB2ENR_UART9EN },
#else
  [STM32H7_MODULE_UART9] = { NULL, 0 },
#endif
#ifdef USART10_BASE
  [STM32H7_MODULE_USART10] = { &RCC->APB2ENR, RCC_APB2ENR_USART10EN },
#else
  [STM32H7_MODULE_USART10] = { NULL, 0 },
#endif
  [STM32H7_MODULE_RNG] = { &RCC->AHB2ENR, RCC_AHB2ENR_RNGEN },
#ifdef RCC_AHB1ENR_ETH1MACEN
  [STM32H7_MODULE_ETH1MAC] = { &RCC->AHB1ENR, RCC_AHB1ENR_ETH1MACEN },
#endif
#ifdef RCC_AHB1ENR_ETH1TXEN
  [STM32H7_MODULE_ETH1TX] = { &RCC->AHB1ENR, RCC_AHB1ENR_ETH1TXEN },
#endif
#ifdef RCC_AHB1ENR_ETH1RXEN
  [STM32H7_MODULE_ETH1RX] = { &RCC->AHB1ENR, RCC_AHB1ENR_ETH1RXEN },
#endif
  [STM32H7_MODULE_USB1_OTG] = { &RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSEN },
  [STM32H7_MODULE_USB1_OTG_ULPI] = { &RCC->AHB1ENR, RCC_AHB1ENR_USB1OTGHSULPIEN },
#ifdef RCC_AHB1ENR_USB20TGHSEN
  [STM32H7_MODULE_USB2_OTG] = { &RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSEN },
#endif
#ifdef RCC_AHB1ENR_USB20TGHSULPIEN
  [STM32H7_MODULE_USB2_OTG_ULPI] = { &RCC->AHB1ENR, RCC_AHB1ENR_USB2OTGHSULPIEN },
#endif
  [STM32H7_MODULE_SDMMC1] = { &RCC->AHB3ENR, RCC_AHB3ENR_SDMMC1EN },
  [STM32H7_MODULE_SDMMC2] = { &RCC->AHB2ENR, RCC_AHB2ENR_SDMMC2EN },
};

void stm32h7_clk_enable(stm32h7_module_index index)
{
  __IO uint32_t *enr;
  uint32_t enable_bit;
  rtems_interrupt_level level;

  enr = stm32h7_clk[index].enr;
  enable_bit = stm32h7_clk[index].enable_bit;

  rtems_interrupt_disable(level);
  SET_BIT(*enr, enable_bit);
  /* Delay after an RCC peripheral clock enabling */
  *enr;
  rtems_interrupt_enable(level);
}

void stm32h7_clk_disable(stm32h7_module_index index)
{
  __IO uint32_t *enr;
  uint32_t enable_bit;
  rtems_interrupt_level level;

  enr = stm32h7_clk[index].enr;
  enable_bit = stm32h7_clk[index].enable_bit;

  rtems_interrupt_disable(level);
  CLEAR_BIT(*enr, enable_bit);
  rtems_interrupt_enable(level);
}

static const stm32h7_clk_info stm32h7_clk_low_power[] = {
  [STM32H7_MODULE_INVALID] = { NULL, 0 },
  [STM32H7_MODULE_GPIOA] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOALPEN },
  [STM32H7_MODULE_GPIOB] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOBLPEN },
  [STM32H7_MODULE_GPIOC] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOCLPEN },
  [STM32H7_MODULE_GPIOD] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIODLPEN },
  [STM32H7_MODULE_GPIOE] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOELPEN },
  [STM32H7_MODULE_GPIOF] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOFLPEN },
  [STM32H7_MODULE_GPIOG] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOGLPEN },
  [STM32H7_MODULE_GPIOH] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOHLPEN },
  [STM32H7_MODULE_GPIOI] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOILPEN },
  [STM32H7_MODULE_GPIOJ] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOJLPEN },
  [STM32H7_MODULE_GPIOK] = { &RCC->AHB4LPENR, RCC_AHB4LPENR_GPIOKLPEN },
  [STM32H7_MODULE_USART1] = { &RCC->APB2LPENR, RCC_APB2LPENR_USART1LPEN },
  [STM32H7_MODULE_USART2] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_USART2LPEN },
  [STM32H7_MODULE_USART3] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_USART3LPEN },
  [STM32H7_MODULE_UART4] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_UART4LPEN },
  [STM32H7_MODULE_UART5] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_UART5LPEN },
  [STM32H7_MODULE_USART6] = { &RCC->APB2LPENR, RCC_APB2LPENR_USART6LPEN },
  [STM32H7_MODULE_UART7] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_UART7LPEN },
  [STM32H7_MODULE_UART8] = { &RCC->APB1LLPENR, RCC_APB1LLPENR_UART8LPEN },
#ifdef UART9_BASE
  [STM32H7_MODULE_UART9] = { &RCC->APB2LPENR, RCC_APB2LPENR_UART9LPEN },
#else
  [STM32H7_MODULE_UART9] = { NULL, 0 },
#endif
#ifdef USART10_BASE
  [STM32H7_MODULE_USART10] = { &RCC->APB2LPENR, RCC_APB2LPENR_USART10LPEN },
#else
  [STM32H7_MODULE_USART10] = { NULL, 0 },
#endif
  [STM32H7_MODULE_RNG] = { &RCC->AHB2LPENR, RCC_AHB2LPENR_RNGLPEN },
#ifdef RCC_AHB1LPENR_ETH1MACLPEN
  [STM32H7_MODULE_ETH1MAC] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_ETH1MACLPEN },
#endif
#ifdef RCC_AHB1LPENR_ETH1TXLPEN
  [STM32H7_MODULE_ETH1TX] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_ETH1TXLPEN },
#endif
#ifdef RCC_AHB1LPENR_ETH1RXLPEN
  [STM32H7_MODULE_ETH1RX] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_ETH1RXLPEN },
#endif
  [STM32H7_MODULE_USB1_OTG] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_USB1OTGHSLPEN },
  [STM32H7_MODULE_USB1_OTG_ULPI] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_USB1OTGHSULPILPEN },
#ifdef RCC_AHB1LPENR_USB2OTGHSLPEN
  [STM32H7_MODULE_USB2_OTG] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_USB2OTGHSLPEN },
#endif
#ifdef RCC_AHB1LPENR_USB2OTGHSULPILPEN
  [STM32H7_MODULE_USB2_OTG_ULPI] = { &RCC->AHB1LPENR, RCC_AHB1LPENR_USB2OTGHSULPILPEN },
#endif
  [STM32H7_MODULE_SDMMC1] = { &RCC->AHB3LPENR, RCC_AHB3LPENR_SDMMC1LPEN },
  [STM32H7_MODULE_SDMMC2] = { &RCC->AHB2LPENR, RCC_AHB2LPENR_SDMMC2LPEN },
};

void stm32h7_clk_low_power_enable(stm32h7_module_index index)
{
  __IO uint32_t *enr;
  uint32_t enable_bit;
  rtems_interrupt_level level;

  enr = stm32h7_clk_low_power[index].enr;
  enable_bit = stm32h7_clk_low_power[index].enable_bit;

  rtems_interrupt_disable(level);
  SET_BIT(*enr, enable_bit);
  /* Delay after an RCC peripheral clock enabling */
  *enr;
  rtems_interrupt_enable(level);
}

void stm32h7_clk_low_power_disable(stm32h7_module_index index)
{
  __IO uint32_t *enr;
  uint32_t enable_bit;
  rtems_interrupt_level level;

  enr = stm32h7_clk_low_power[index].enr;
  enable_bit = stm32h7_clk_low_power[index].enable_bit;

  rtems_interrupt_disable(level);
  CLEAR_BIT(*enr, enable_bit);
  rtems_interrupt_enable(level);
}

void stm32h7_gpio_init(const stm32h7_gpio_config *config)
{
  stm32h7_module_index index;

  index = stm32h7_get_module_index(config->regs);
  stm32h7_clk_enable(index);
  HAL_GPIO_Init(config->regs, &config->config);
}

void stm32h7_uart_polled_write(rtems_termios_device_context *base, char c)
{
  stm32h7_uart_context *ctx;
  USART_TypeDef *regs;

  ctx = stm32h7_uart_get_context(base);
  regs = ctx->uart.Instance;

  while ((regs->ISR & USART_ISR_TXE_TXFNF) == 0) {
    /* Wait */
  }

  regs->TDR = (uint8_t) c;
}

int stm32h7_uart_polled_read(rtems_termios_device_context *base)
{
  stm32h7_uart_context *ctx;
  USART_TypeDef *regs;

  ctx = stm32h7_uart_get_context(base);
  regs = ctx->uart.Instance;

  if ((regs->ISR & USART_ISR_RXNE_RXFNE) == 0) {
    return -1;
  }

  return (uint8_t) regs->RDR;
}
