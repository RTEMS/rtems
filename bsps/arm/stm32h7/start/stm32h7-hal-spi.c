/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32H7
 *
 * @brief This source file contains the SPI MSP initialization implementation.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR) Corporation
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

void HAL_SPI_MspInit(SPI_HandleTypeDef *spi)
{
  stm32h7_spi_context *ctx;
  const stm32h7_spi_config *config;
  stm32h7_module_index index = stm32h7_get_module_index(spi->Instance);

  ctx = RTEMS_CONTAINER_OF(spi, stm32h7_spi_context, spi);
  config = ctx->config;
  stm32h7_clk_enable(index);
  stm32h7_gpio_init(&config->sck_gpio);
  stm32h7_gpio_init(&config->miso_gpio);
  stm32h7_gpio_init(&config->mosi_gpio);

  /* Configure SPI CS GPIOs */
  for (int i = 0; i < STM32H7_NUM_SOFT_CS; i++) {
    if (config->cs_gpio[i].regs == NULL) {
      continue;
    }
    /* TODO(kmoore) handle multiple pins in a single GPIO block */

    /* configure GPIO CS and set output high */
    stm32h7_gpio_init(&config->cs_gpio[i]);
    /* Set all GPIO CS pins high */
    HAL_GPIO_WritePin(config->cs_gpio[i].regs, config->cs_gpio[i].config.Pin, GPIO_PIN_SET);
  }
}
