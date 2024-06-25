/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32U5
 *
 * @brief Oscillator configuration for the STM32U5
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

const RCC_OscInitTypeDef stm32u5_config_oscillator = {
  .OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI |
                    RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_MSI |
                    RCC_OSCILLATORTYPE_LSE,
  .HSEState            = RCC_HSE_ON,
  .HSIState            = RCC_HSI_ON,
  .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
  .LSIState            = RCC_LSI_ON,
  .MSIState            = RCC_MSI_ON,
  .LSEState            = RCC_LSE_ON_RTC_ONLY,
  .MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT,
  .MSIClockRange       = RCC_MSIRANGE_0,
  .LSIDiv              = RCC_LSI_DIV1,
  .PLL.PLLState        = RCC_PLL_ON,
  .PLL.PLLSource       = RCC_PLLSOURCE_HSE,
  .PLL.PLLMBOOST       = RCC_PLLMBOOST_DIV1,
  .PLL.PLLM            = 1,
  .PLL.PLLN            = 20,
  .PLL.PLLP            = 5,
  .PLL.PLLQ            = 2,
  .PLL.PLLR            = 2,
  .PLL.PLLRGE          = RCC_PLLVCIRANGE_1,
  .PLL.PLLFRACN        = 0
};
