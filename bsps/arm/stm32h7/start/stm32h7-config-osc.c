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
