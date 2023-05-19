/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief MMU early configuration.
 */

/*
 * Copyright (C) 2011, 2013 embedded brains GmbH & Co. KG
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
