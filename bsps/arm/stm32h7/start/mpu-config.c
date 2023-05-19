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

#include <bsp/linker-symbols.h>
#include <stm32h7/memory.h>
#include <stm32h7/mpu-config.h>

const ARMV7M_MPU_Region_config stm32h7_config_mpu_region [] = {
    {
      .begin = stm32h7_memory_sram_axi_begin,
      .end = stm32h7_memory_sram_axi_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = stm32h7_memory_sdram_1_begin,
      .end = stm32h7_memory_sdram_1_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = stm32h7_memory_sdram_2_begin,
      .end = stm32h7_memory_sdram_2_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = bsp_section_start_begin,
      .end = bsp_section_text_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x5)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = bsp_section_rodata_begin,
      .end = bsp_section_rodata_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x5)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = bsp_section_nocache_begin,
      .end = bsp_section_nocachenoload_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x2)
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = stm32h7_memory_null_begin,
      .end = stm32h7_memory_null_end,
      .rasr = ARMV7M_MPU_RASR_XN | ARMV7M_MPU_RASR_ENABLE,
    }
  };

const size_t stm32h7_config_mpu_region_count =
  RTEMS_ARRAY_SIZE(stm32h7_config_mpu_region);
