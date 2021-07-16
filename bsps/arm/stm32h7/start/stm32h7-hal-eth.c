/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <bspopts.h>

static const stm32h7_gpio_config gpiog = {
  .regs = GPIOG,
  .config = {
    .Pin = STM32H7_ETH_GPIOG_PINS,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Alternate = GPIO_AF11_ETH
  }
};

static const stm32h7_gpio_config gpioc = {
  .regs = GPIOC,
  .config = {
    .Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Alternate = GPIO_AF11_ETH
  }
};

static const stm32h7_gpio_config gpioa = {
  .regs = GPIOA,
  .config = {
    .Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Alternate = GPIO_AF11_ETH
  }
};

#ifdef STM32H7_ETH_GPIOB_PINS

static const stm32h7_gpio_config gpiob = {
  .regs = GPIOB,
  .config = {
    .Pin = STM32H7_ETH_GPIOB_PINS,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
    .Alternate = GPIO_AF11_ETH
  }
};

#endif

void
HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  stm32h7_clk_enable(STM32H7_MODULE_ETH1MAC);
  stm32h7_clk_enable(STM32H7_MODULE_ETH1TX);
  stm32h7_clk_enable(STM32H7_MODULE_ETH1RX);
  stm32h7_gpio_init(&gpiog);
  stm32h7_gpio_init(&gpioc);
  stm32h7_gpio_init(&gpioa);
#ifdef STM32H7_ETH_GPIOB_PINS
  stm32h7_gpio_init(&gpiob);
#endif
}
