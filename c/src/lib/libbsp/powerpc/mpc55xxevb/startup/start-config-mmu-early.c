/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MMU early configuration.
 */

/*
 * Copyright (c) 2011-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/mpc55xx-config.h>

const struct MMU_tag mpc55xx_start_config_mmu_early [] = {
#if (defined(MPC55XX_BOARD_MPC5674F_ECU508) \
  || defined(MPC55XX_BOARD_MPC5674F_RSM6)) \
    && !defined(MPC55XX_NEEDS_LOW_LEVEL_INIT)
  /* Used as cache-inhibited area later (ADC, DSPI queues) */
  MPC55XX_MMU_TAG_INITIALIZER(14, 0x4003c000, MPC55XX_MMU_16K, 0, 1, 1, 0)
#elif MPC55XX_CHIP_FAMILY == 555
  /* Internal SRAM 96k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_256K, 1, 1, 1, 0),
#elif MPC55XX_CHIP_FAMILY == 556
  /* Internal SRAM 128k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 1, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x40010000, MPC55XX_MMU_64K, 0, 1, 1, 0),
#elif MPC55XX_CHIP_FAMILY == 564
  /* Internal flash 1M */
  MPC55XX_MMU_TAG_INITIALIZER(0, 0x00000000, MPC55XX_MMU_1M, 1, 0, 1, 1),
  /* IO */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0xffe00000, MPC55XX_MMU_2M, 0, 1, 1, 1),
  MPC55XX_MMU_TAG_INITIALIZER(2, 0xc3f00000, MPC55XX_MMU_1M, 0, 1, 1, 1),
  /* Internal SRAM 64k + 64k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 1, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(4, 0x50000000, MPC55XX_MMU_64K, 0, 1, 1, 0)
#elif MPC55XX_CHIP_FAMILY == 566
  /* Internal flash 2M */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0x00000000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(4, 0x00100000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  /* IO */
  MPC55XX_MMU_TAG_INITIALIZER(2, 0xc3f00000, MPC55XX_MMU_1M, 0, 1, 1, 1),
  /* Internal SRAM 512k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_256K, 1, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x40040000, MPC55XX_MMU_256K, 1, 1, 1, 0)
#elif MPC55XX_CHIP_FAMILY == 567
  /* Internal SRAM 256k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_256K, 1, 1, 1, 0)
#endif
};

const size_t mpc55xx_start_config_mmu_early_count [] = {
  RTEMS_ARRAY_SIZE(mpc55xx_start_config_mmu_early)
};
