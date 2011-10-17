/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start EMC static memory configuration.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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
 *
 * $Id$
 */

#include <bsp/start-config.h>
#include <bsp/lpc24xx.h>

const BSP_START_DATA_SECTION lpc24xx_emc_static_chip_config
   lpc24xx_start_config_emc_static_chip [] = {
#ifdef LPC24XX_EMC_NUMONYX
  /*
   * Static Memory 1: Numonyx M29W160EB
   *
   * 1 clock cycle = 1/72MHz = 13.9ns
   */
  {
    .chip_select = (volatile lpc_emc_static *) EMC_STA_BASE_1,
    .config = {
      /*
       * 16 bit, page mode disabled, active LOW chip select, extended wait
       * disabled, writes not protected, byte lane state LOW/LOW (!).
       */
      .config = 0x81,

      /* 1 clock cycles delay from the chip select 1 to the write enable */
      .waitwen = 0,

      /*
       * 0 clock cycles delay from the chip select 1 or address change
       * (whichever is later) to the output enable
       */
      .waitoen = 0,

      /* 7 clock cycles delay from the chip select 1 to the read access */
      .waitrd = 0x6,

      /*
       * 32 clock cycles delay for asynchronous page mode sequential accesses
       */
      .waitpage = 0x1f,

      /* 5 clock cycles delay from the chip select 1 to the write access */
      .waitwr = 0x3,

      /* 16 bus turnaround cycles */
      .waitrun = 0xf
    }
  }
#endif /* LPC24XX_EMC_NUMONYX */
};

const BSP_START_DATA_SECTION size_t
  lpc24xx_start_config_emc_static_chip_count =
    sizeof(lpc24xx_start_config_emc_static_chip)
      / sizeof(lpc24xx_start_config_emc_static_chip [0]);
