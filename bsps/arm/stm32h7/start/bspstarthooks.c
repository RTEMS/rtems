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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <stm32h7/hal.h>
#include <stm32h7/memory.h>
#include <stm32h7/mpu-config.h>
#include <rtems/score/armv7m.h>

#include <string.h>

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
}

static void init_power(void)
{
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  __HAL_PWR_VOLTAGESCALING_CONFIG(stm32h7_config_pwr_regulator_voltagescaling);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    /* Wait */
  }
}

static void init_oscillator(void)
{
  HAL_StatusTypeDef status;

  status = HAL_RCC_OscConfig(&stm32h7_config_oscillator);
  if (status != HAL_OK) {
    bsp_reset();
  }
}

static void init_clocks(void)
{
  HAL_StatusTypeDef status;

  status = HAL_RCC_ClockConfig(
    &stm32h7_config_clocks,
    stm32h7_config_flash_latency
  );
  if (status != HAL_OK) {
    bsp_reset();
  }
}

static void init_peripheral_clocks(void)
{
  HAL_StatusTypeDef status;

  status = HAL_RCCEx_PeriphCLKConfig(&stm32h7_config_peripheral_clocks);
  if (status != HAL_OK) {
    bsp_reset();
  }
}

void bsp_start_hook_0(void)
{
  if ((RCC->AHB3ENR & RCC_AHB3ENR_FMCEN) == 0) {
    /*
     * Only perform the low-level initialization if necessary.  An initialized
     * FMC indicates that a boot loader already performed the low-level
     * initialization.
     */
    SystemInit();
    init_power();
    init_oscillator();
    init_clocks();
    init_peripheral_clocks();
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
    HAL_Init();
    SystemInit_ExtMemCtl();
  }

  if ((SCB->CCR & SCB_CCR_IC_Msk) == 0) {
    SCB_EnableICache();
  }

  if ((SCB->CCR & SCB_CCR_DC_Msk) == 0) {
    SCB_EnableDCache();
  }

  _ARMV7M_MPU_Setup(stm32h7_config_mpu_region, stm32h7_config_mpu_region_count);
}

void bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
  SCB_CleanDCache();
  SCB_InvalidateICache();
  bsp_start_clear_bss();
}
