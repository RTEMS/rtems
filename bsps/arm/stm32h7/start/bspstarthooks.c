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

static uint32_t get_region_size(uintptr_t size)
{
  if ((size & (size - 1)) == 0) {
    return ARMV7M_MPU_RASR_SIZE(30 - __builtin_clz(size));
  } else {
    return ARMV7M_MPU_RASR_SIZE(31 - __builtin_clz(size));
  }
}

static void set_region(
  volatile ARMV7M_MPU *mpu,
  uint32_t region,
  uint32_t rasr,
  const void *begin,
  const void *end
)
{
  uintptr_t size;
  uint32_t rbar;

  RTEMS_OBFUSCATE_VARIABLE(begin);
  RTEMS_OBFUSCATE_VARIABLE(end);
  size = (uintptr_t) end - (uintptr_t) begin;

  if ( size > 0 ) {
    rbar = (uintptr_t) begin | region | ARMV7M_MPU_RBAR_VALID;
    rasr |= get_region_size(size);
  } else {
    rbar = region;
    rasr = 0;
  }

  mpu->rbar = rbar;
  mpu->rasr = rasr;
}

static void init_mpu(void)
{
  volatile ARMV7M_MPU *mpu;
  volatile ARMV7M_SCB *scb;
  uint32_t region_count;
  uint32_t region;

  mpu = _ARMV7M_MPU;
  scb = _ARMV7M_SCB;

  region_count = ARMV7M_MPU_TYPE_DREGION_GET(mpu->type);

  for (region = 0; region < region_count; ++region) {
    mpu->rbar = ARMV7M_MPU_RBAR_VALID | region;
    mpu->rasr = 0;
  }

  set_region(
    mpu,
    0,
    ARMV7M_MPU_RASR_XN
      | ARMV7M_MPU_RASR_AP(0x3)
      | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
      | ARMV7M_MPU_RASR_ENABLE,
    stm32h7_memory_sram_axi_begin,
    stm32h7_memory_sram_axi_end
  );
  set_region(
    mpu,
    1,
    ARMV7M_MPU_RASR_XN
      | ARMV7M_MPU_RASR_AP(0x3)
      | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
      | ARMV7M_MPU_RASR_ENABLE,
    stm32h7_memory_sdram_1_begin,
    stm32h7_memory_sdram_1_end
  );
  set_region(
    mpu,
    2,
    ARMV7M_MPU_RASR_AP(0x5)
      | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
      | ARMV7M_MPU_RASR_ENABLE,
    bsp_section_start_begin,
    bsp_section_text_end
  );
  set_region(
    mpu,
    3,
    ARMV7M_MPU_RASR_XN
      | ARMV7M_MPU_RASR_AP(0x5)
      | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
      | ARMV7M_MPU_RASR_ENABLE,
    bsp_section_rodata_begin,
    bsp_section_rodata_end
  );
  set_region(
    mpu,
    4,
    ARMV7M_MPU_RASR_XN
      | ARMV7M_MPU_RASR_AP(0x3)
      | ARMV7M_MPU_RASR_TEX(0x2)
      | ARMV7M_MPU_RASR_ENABLE,
    bsp_section_nocache_begin,
    bsp_section_nocachenoload_end
  );
  set_region(
    mpu,
    region - 1,
    ARMV7M_MPU_RASR_XN | ARMV7M_MPU_RASR_ENABLE,
    stm32h7_memory_null_begin,
    stm32h7_memory_null_end
  );

  mpu->ctrl = ARMV7M_MPU_CTRL_ENABLE | ARMV7M_MPU_CTRL_PRIVDEFENA;
  scb->shcsr |= ARMV7M_SCB_SHCSR_MEMFAULTENA;

  _ARM_Data_synchronization_barrier();
  _ARM_Instruction_synchronization_barrier();
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

  init_mpu();
}

void bsp_start_hook_1(void)
{
  bsp_start_copy_sections_compact();
  SCB_CleanDCache();
  SCB_InvalidateICache();
  bsp_start_clear_bss();
}
