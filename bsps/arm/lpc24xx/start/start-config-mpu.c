/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief BSP start MPU configuration.
 */

/*
 * Copyright (C) 2011, 2019 embedded brains GmbH & Co. KG
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

#include <bsp/start-config.h>

#ifdef ARM_MULTILIB_ARCH_V7M
  BSP_START_DATA_SECTION const ARMV7M_MPU_Region
    lpc24xx_start_config_mpu_region [] = {
      #if defined(LPC24XX_EMC_IS42S32800D7) \
        || defined(LPC24XX_EMC_IS42S32800B) \
        || defined(LPC24XX_EMC_MT48LC4M16A2) \
        || defined(LPC24XX_EMC_W9825G2JB75I)
        ARMV7M_MPU_REGION_INITIALIZER(
          0,
          0x00000000,
          ARMV7M_MPU_SIZE_512_KB,
          ARMV7M_MPU_ATTR_RX
        ),
        ARMV7M_MPU_REGION_INITIALIZER(
          1,
          0x10000000,
          ARMV7M_MPU_SIZE_64_KB,
          ARMV7M_MPU_ATTR_RWX
        ),
        ARMV7M_MPU_REGION_INITIALIZER(
          2,
          0x20000000,
          ARMV7M_MPU_SIZE_32_KB,
          ARMV7M_MPU_ATTR_RWX
        ),
        ARMV7M_MPU_REGION_INITIALIZER(
          3,
          0xa0000000,
          #if defined(LPC24XX_EMC_MT48LC4M16A2)
            ARMV7M_MPU_SIZE_8_MB,
          #else
            ARMV7M_MPU_SIZE_32_MB,
          #endif
          ARMV7M_MPU_ATTR_RWX
        ),
        ARMV7M_MPU_REGION_INITIALIZER(
          4,
          0x20080000,
          ARMV7M_MPU_SIZE_128_KB,
          ARMV7M_MPU_ATTR_IO
        ),
        ARMV7M_MPU_REGION_INITIALIZER(
          5,
          0x40000000,
          ARMV7M_MPU_SIZE_1_MB,
          ARMV7M_MPU_ATTR_IO
        ),
        #if defined(LPC24XX_EMC_M29W320E70) \
          || defined(LPC24XX_EMC_SST39VF3201)
          ARMV7M_MPU_REGION_INITIALIZER(
            6,
            0x80000000,
            ARMV7M_MPU_SIZE_4_MB,
            ARMV7M_MPU_ATTR_RWX
          ),
        #else
          ARMV7M_MPU_REGION_DISABLED_INITIALIZER(6),
        #endif
        ARMV7M_MPU_REGION_DISABLED_INITIALIZER(7)
     #endif
  };

  BSP_START_DATA_SECTION const size_t
    lpc24xx_start_config_mpu_region_count =
      sizeof(lpc24xx_start_config_mpu_region)
        / sizeof(lpc24xx_start_config_mpu_region [0]);
#endif
