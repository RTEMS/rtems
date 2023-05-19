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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <stm32h7/hal.h>
#include <stm32h7/memory.h>
#include <stm32h7/mpu-config.h>
#include <rtems/score/armv7m.h>

#include <string.h>

#include <stm32h747i_eval_qspi.h>
static BSP_QSPI_Init_t QSPinit;

void stm32h7_init_qspi(void)
{
#if defined(STM32H7_MEMORY_QUADSPI_SIZE) && STM32H7_MEMORY_QUADSPI_SIZE > 0
    /* let's initialize Quad SPI memory here for memory mapped mode */
    /* due to usage of static QSPinit variable please call this function
       after bsp_start_clear_bss call since otherwise you would hit uninitialized
       variable memory while accessing it and in addition the call to bsp_start_clear_bss
       would wipe the variable content later after its initialization here. */
    BSP_QSPI_Init(0, &QSPinit);
    BSP_QSPI_EnableMemoryMappedMode(0);
#endif
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
    stm32h7_init_power();
    stm32h7_init_oscillator();
    stm32h7_init_clocks();
    stm32h7_init_peripheral_clocks();
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
    HAL_Init();
    SystemInit_ExtMemCtl();
  }

#if __CORTEX_M == 0x07U
  if ((SCB->CCR & SCB_CCR_IC_Msk) == 0) {
    SCB_EnableICache();
  }

  if ((SCB->CCR & SCB_CCR_DC_Msk) == 0) {
    SCB_EnableDCache();
  }

  _ARMV7M_MPU_Setup(ARMV7M_MPU_CTRL_DEFAULT, stm32h7_config_mpu_region, stm32h7_config_mpu_region_count);
#endif
}

void bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
#if __CORTEX_M == 0x07U
  SCB_CleanDCache();
  SCB_InvalidateICache();
#endif
  bsp_start_clear_bss();
  stm32h7_init_qspi();
}
