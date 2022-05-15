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

const RCC_PeriphCLKInitTypeDef stm32h7_config_peripheral_clocks = {
  .PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_USART3
    | RCC_PERIPHCLK_FDCAN | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1
    | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_FMC | RCC_PERIPHCLK_RNG,
  .PLL2.PLL2M = 3,
  .PLL2.PLL2N = 48,
  .PLL2.PLL2P = 1,
  .PLL2.PLL2Q = 2,
  .PLL2.PLL2R = 2,
  .PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3,
  .PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE,
  .PLL2.PLL2FRACN = 0,
  .PLL3.PLL3M = 25,
  .PLL3.PLL3N = 192,
  .PLL3.PLL3P = 2,
  .PLL3.PLL3Q = 4,
  .PLL3.PLL3R = 2,
  .PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0,
  .PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE,
  .PLL3.PLL3FRACN = 0,
  .FmcClockSelection = RCC_FMCCLKSOURCE_PLL2,
  .FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL,
  .Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1,
  .Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2,
  .I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1,
  .UsbClockSelection = RCC_USBCLKSOURCE_PLL3,
  .RTCClockSelection = RCC_RTCCLKSOURCE_LSE,
  .RngClockSelection = RCC_RNGCLKSOURCE_HSI48
};
