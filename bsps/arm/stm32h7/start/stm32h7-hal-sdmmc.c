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

static const stm32h7_gpio_config gpiob = {
  .regs = GPIOB,
  .config = {
    .Pin = GPIO_PIN_8 | GPIO_PIN_9,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .Alternate = GPIO_AF7_SDMMC1
  }
};

static const stm32h7_gpio_config gpioc_af12 = {
  .regs = GPIOC,
  .config = {
    .Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .Alternate = GPIO_AF12_SDMMC1
  }
};

static const stm32h7_gpio_config gpioc_af8 = {
  .regs = GPIOC,
  .config = {
    .Pin = GPIO_PIN_6 | GPIO_PIN_7,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .Alternate = GPIO_AF8_SDMMC1
  }
};

static const stm32h7_gpio_config gpiod = {
  .regs = GPIOD,
  .config = {
    .Pin = GPIO_PIN_2,
    .Mode = GPIO_MODE_AF_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
    .Alternate = GPIO_AF12_SDMMC1
  }
};

void
HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
  stm32h7_clk_enable(STM32H7_MODULE_SDMMC1);
  stm32h7_gpio_init(&gpiob);
  stm32h7_gpio_init(&gpioc_af12);
  stm32h7_gpio_init(&gpioc_af8);
  stm32h7_gpio_init(&gpiod);
  __HAL_RCC_SDMMC1_FORCE_RESET();
  __HAL_RCC_SDMMC1_RELEASE_RESET();
}
