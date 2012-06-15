/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start MPU configuration.
 */

/*
 * Copyright (c) 2011-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/start-config.h>

#ifdef ARM_MULTILIB_ARCH_V7M
  BSP_START_DATA_SECTION const ARMV7M_MPU_Region
    lpc24xx_start_config_mpu_region [] = {
      #if defined(LPC24XX_EMC_IS42S32800D7) \
        || defined(LPC24XX_EMC_W9825G2JB75I) \
        || defined(LPC24XX_EMC_IS42S32800B)
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
          ARMV7M_MPU_SIZE_32_MB,
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
