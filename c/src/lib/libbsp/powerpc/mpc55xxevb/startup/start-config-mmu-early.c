/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MMU early configuration.
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

#include <bsp/start.h>
#include <bsp/mpc55xx-config.h>

BSP_START_TEXT_SECTION const struct MMU_tag
  mpc55xx_start_config_mmu_early [] = {
#if MPC55XX_CHIP_TYPE / 10 == 555
  /* Internal SRAM 96k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_256K, 1, 1, 1, 0),
#elif MPC55XX_CHIP_TYPE / 10 == 556
  /* Internal SRAM 128k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 1, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x40010000, MPC55XX_MMU_64K, 0, 1, 1, 0),
#elif MPC55XX_CHIP_TYPE / 10 == 564
  /* Internal flash 1M */
  MPC55XX_MMU_TAG_INITIALIZER(0, 0x00000000, MPC55XX_MMU_1M, 1, 0, 1, 1),
  /* IO */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0xffe00000, MPC55XX_MMU_2M, 0, 1, 1, 1),
  MPC55XX_MMU_TAG_INITIALIZER(2, 0xc3f00000, MPC55XX_MMU_1M, 0, 1, 1, 1),
  /* Internal SRAM 64k + 64k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 1, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(4, 0x50000000, MPC55XX_MMU_64K, 0, 1, 1, 0)
#elif MPC55XX_CHIP_TYPE / 10 == 567
  /* Internal SRAM 256k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_256K, 1, 1, 1, 0)
#endif
};

BSP_START_TEXT_SECTION const size_t mpc55xx_start_config_mmu_early_count [] = {
  sizeof(mpc55xx_start_config_mmu_early)
    / sizeof(mpc55xx_start_config_mmu_early [0])
};
