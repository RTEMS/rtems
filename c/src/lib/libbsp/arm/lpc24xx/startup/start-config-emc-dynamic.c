/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start EMC dynamic memory configuration.
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

const BSP_START_DATA_SECTION lpc24xx_emc_dynamic_config
  lpc24xx_start_config_emc_dynamic [] = {
#if defined(LPC24XX_EMC_MICRON)
  /* Dynamic Memory 0: Micron M T48LC 4M16 A2 P 75 IT */
  {
    /* Auto-refresh command every 15.6 us */
    .refresh = 0x46,

    /* Use command delayed strategy */
    .readconfig = 1,

    /* Precharge command period 20 ns */
    .trp = 1,

    /* Active to precharge command period 44 ns */
    .tras = 3,

    /* FIXME */
    .tsrex = 5,

    /* FIXME */
    .tapr = 2,

    /* Data-in to active command period tWR + tRP */
    .tdal = 4,

    /* Write recovery time 15 ns */
    .twr = 1,

    /* Active to active command period 66 ns */
    .trc = 4,

    /* Auto refresh period 66 ns */
    .trfc = 4,

    /* Exit self refresh to active command period 75 ns */
    .txsr = 5,

    /* Active bank a to active bank b command period 15 ns */
    .trrd = 1,

    /* Load mode register to active or refresh command period 2 tCK */
    .tmrd = 1
  }
#elif defined(LPC24XX_EMC_K4S561632E)
  {
    .refresh = 35,
    .readconfig = 1,
    .trp = 2,
    .tras = 4,
    .tsrex = 5,
    .tapr = 1,
    .tdal = 5,
    .twr = 3,
    .trc = 5,
    .trfc = 5,
    .txsr = 5,
    .trrd = 3,
    .tmrd = 2
  }
#endif
};

const BSP_START_DATA_SECTION lpc24xx_emc_dynamic_chip_config
  lpc24xx_start_config_emc_dynamic_chip [] = {
#if defined(LPC24XX_EMC_MICRON)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,

    /*
     * Use SDRAM, 0 0 001 01 address mapping, disabled buffer, unprotected
     * writes.
     */
    .config = 0x280,

    .rascas = EMC_DYN_RASCAS_RAS(2) | EMC_DYN_RASCAS_CAS(2, 0),
    .mode = 0xa0000000 | (0x23 << (1 + 2 + 8))
  }
#elif defined(LPC24XX_EMC_K4S561632E)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,
    .config = 0x680,
    .rascas = EMC_DYN_RASCAS_RAS(3) | EMC_DYN_RASCAS_CAS(3, 0),
    .mode = 0xa0000000 | (0x33 << 12)
  }
#endif
};

const BSP_START_DATA_SECTION size_t
  lpc24xx_start_config_emc_dynamic_chip_count =
    sizeof(lpc24xx_start_config_emc_dynamic_chip)
      / sizeof(lpc24xx_start_config_emc_dynamic_chip [0]);
