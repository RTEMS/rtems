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

const uint32_t stm32h7_config_pwr_regulator_voltagescaling =
  PWR_REGULATOR_VOLTAGE_SCALE0;

const RCC_OscInitTypeDef stm32h7_config_oscillator = {
  .OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE
    | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI48,
  .HSEState = RCC_HSE_ON,
  .LSEState = RCC_LSE_ON,
  .HSIState = RCC_HSI_DIV1,
  .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
  .HSI48State = RCC_HSI48_ON,
  .PLL.PLLState = RCC_PLL_ON,
  .PLL.PLLSource = RCC_PLLSOURCE_HSE,
  .PLL.PLLM = 5,
  .PLL.PLLN = 192,
  .PLL.PLLP = 2,
  .PLL.PLLQ = 12,
  .PLL.PLLR = 2,
  .PLL.PLLRGE = RCC_PLL1VCIRANGE_2,
  .PLL.PLLVCOSEL = RCC_PLL1VCOWIDE,
  .PLL.PLLFRACN = 0
};

const RCC_ClkInitTypeDef stm32h7_config_clocks = {
  .ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
    | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1,
  .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
  .SYSCLKDivider = RCC_SYSCLK_DIV1,
  .AHBCLKDivider = RCC_HCLK_DIV2,
  .APB3CLKDivider = RCC_APB3_DIV2,
  .APB1CLKDivider = RCC_APB1_DIV2,
  .APB2CLKDivider = RCC_APB2_DIV2,
  .APB4CLKDivider = RCC_APB4_DIV2
};

const uint32_t stm32h7_config_flash_latency = FLASH_LATENCY_4;

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
