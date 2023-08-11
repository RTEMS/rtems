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

#include <imxrt/memory.h>
#include <imxrt/mpu-config.h>
#include <rtems/score/armv7m.h>

BSP_START_DATA_SECTION const ARMV7M_MPU_Region_config
  imxrt_config_mpu_region [] = {
    {
      .begin = imxrt_memory_extram_begin,
      .end = imxrt_memory_extram_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_ocram_begin,
      .end = imxrt_memory_ocram_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_flash_raw_begin,
      .end = imxrt_memory_flash_raw_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1) | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_extram_nocache_begin,
      .end = imxrt_memory_extram_nocache_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1)
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_ocram_nocache_begin,
      .end = imxrt_memory_ocram_nocache_end,
      .rasr = ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x1)
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_peripheral_begin,
      .end = imxrt_memory_peripheral_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x3)
        | ARMV7M_MPU_RASR_TEX(0x2)
        | ARMV7M_MPU_RASR_ENABLE,
    }, {
      .begin = imxrt_memory_null_begin,
      .end = imxrt_memory_null_end,
      .rasr = ARMV7M_MPU_RASR_XN
        | ARMV7M_MPU_RASR_AP(0x0)
        | ARMV7M_MPU_RASR_ENABLE,
    }
  };

BSP_START_DATA_SECTION const size_t imxrt_config_mpu_region_count =
  RTEMS_ARRAY_SIZE(imxrt_config_mpu_region);
