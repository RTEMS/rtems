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

BSP_START_DATA_SECTION const lpc24xx_emc_dynamic_config
  lpc24xx_start_config_emc_dynamic [] = {
#if defined(LPC24XX_EMC_MT48LC4M16A2)
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
#elif defined(LPC24XX_EMC_IS42S32800D7)
  /* Dynamic Memory 0: ISSI IS42S32800D7 at 51612800Hz (tCK = 19.4ns) */
  {
    /* (n * 16) clock cycles -> 15.5us <= 15.6 us */
    .refresh = 50,

    /* Use command delayed strategy */
    .readconfig = 1,

    /* (n + 1) clock cycles -> 38.8ns >= 20ns */
    .trp = 1,

    /* (n + 1) clock cycles -> 58.1ns >= 45ns */
    .tras = 2,

    /* (n + 1) clock cycles -> 77.5ns >= 70ns (tXSR) */
    .tsrex = 3,

    /* (n + 1) clock cycles -> 38.8ns >= 20ns (tRCD) */
    .tapr = 1,

    /* n clock cycles -> 38.8ns >= 35ns */
    .tdal = 2,

    /* (n + 1) clock cycles = 19.4ns >= 14ns (tDPL) */
    .twr = 0,

    /* (n + 1) clock cycles = 77.5ns >= 67.5ns */
    .trc = 3,

    /* (n + 1) clock cycles = 77.5ns >= 67.5ns (tRC) */
    .trfc = 3,

    /* (n + 1) clock cycles = 77.5ns >= 70ns */
    .txsr = 3,

    /* (n + 1) clock cycles = 19.4ns >= 14ns */
    .trrd = 0,

    /* (n + 1) clock cycles = 19.4ns >= 14ns */
    .tmrd = 0
  }
#elif defined(LPC24XX_EMC_W9825G2JB75I)
  /* Dynamic Memory 0: Winbond W9825G2JB75I at 51612800Hz (tCK = 19.4ns) */
  {
    /* (n * 16) clock cycles -> 15.5us <= 15.6 us */
    .refresh = 50,

    /* Use command delayed strategy */
    .readconfig = 1,

    /* (n + 1) clock cycles -> 38.8ns >= 20ns */
    .trp = 1,

    /* (n + 1) clock cycles -> 58.1ns >= 45ns */
    .tras = 2,

    /* (n + 1) clock cycles -> 77.5ns >= 75ns (tXSR) */
    .tsrex = 3,

    /* (n + 1) clock cycles -> 38.8ns >= 20ns (tRCD) */
    .tapr = 1,

    /* n clock cycles -> 77.5ns >= tWR + tRP -> 2 * tCK + 20ns */
    .tdal = 4,

    /* (n + 1) clock cycles == 2 * tCK */
    .twr = 1,

    /* (n + 1) clock cycles = 77.5ns >= 65ns */
    .trc = 3,

    /* (n + 1) clock cycles = 77.5ns >= 65ns (tRC) */
    .trfc = 3,

    /* (n + 1) clock cycles = 77.5ns >= 75ns */
    .txsr = 3,

    /* (n + 1) clock cycles == 2 * tCK */
    .trrd = 1,

    /* (n + 1) clock cycles == 2 * tCK (tRSC)*/
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
#elif defined(LPC24XX_EMC_IS42S32800B)
  #if LPC24XX_EMCCLK == 72000000U
    {
      /* tCK = 13.888ns at 72MHz */

      /* (n * 16) clock cycles -> 15.556us <= 15.6us */
      .refresh = 70,

      .readconfig = 1,

      /* (n + 1) clock cycles -> 27.8ns >= 20ns */
      .trp = 1,

      /* (n + 1) clock cycles -> 55.5ns >= 45ns */
      .tras = 3,

      /* (n + 1) clock cycles -> 69.4ns >= 70ns (tRC) */
      .tsrex = 5,

      /* (n + 1) clock cycles -> 41.7ns >= FIXME */
      .tapr = 2,

      /* n clock cycles -> 55.5ns >= tWR + tRP = 47.8ns */
      .tdal = 4,

      /* (n + 1) clock cycles == 2 * tCK */
      .twr = 1,

      /* (n + 1) clock cycles -> 83.3ns >= 70ns */
      .trc = 5,

      /* (n + 1) clock cycles -> 83.3ns >= 70ns */
      .trfc = 5,

      /* (n + 1) clock cycles -> 69.4ns >= 70ns (tRC) */
      .txsr = 5,

      /* (n + 1) clock cycles -> 27.8ns >= 14ns */
      .trrd = 1,

      /* (n + 1) clock cycles == 2 * tCK */
      .tmrd = 1,

      /* FIXME */
      .emcdlyctl = 0x1112
    }
  #elif LPC24XX_EMCCLK == 60000000U
    {
      .refresh = 0x3a,
      .readconfig = 1,
      .trp = 1,
      .tras = 3,
      .tsrex = 5,
      .tapr = 2,
      .tdal = 3,
      .twr = 1,
      .trc = 4,
      .trfc = 4,
      .txsr = 5,
      .trrd = 1,
      .tmrd = 1,
      .emcdlyctl = 0x1112
    }
  #else
    #error "unexpected EMCCLK"
  #endif
#endif
};

BSP_START_DATA_SECTION const lpc24xx_emc_dynamic_chip_config
  lpc24xx_start_config_emc_dynamic_chip [] = {
#if defined(LPC24XX_EMC_MT48LC4M16A2)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,

    /*
     * Use SDRAM, 0 0 001 01 address mapping, disabled buffer, unprotected
     * writes.  4 banks, 12 row lines, 8 column lines.
     */
    .config = 0x280,

    .rascas = EMC_DYN_RASCAS_RAS(2) | EMC_DYN_RASCAS_CAS(2, 0),
    .mode = 0xa0000000 | (0x23 << (1 + 2 + 8))
  }
#elif defined(LPC24XX_EMC_W9825G2JB75I) \
  || defined(LPC24XX_EMC_IS42S32800D7)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,

    /* 32-bit data bus, 4 banks, 12 row lines, 9 column lines, RBC */
    .config = 0x4480,

    /* RAS based on tRCD = 20ns */
    .rascas = EMC_DYN_RASCAS_RAS(2) | EMC_DYN_RASCAS_CAS(2, 0),

    /* CAS 2, burst length 4 */
    .mode = 0xa0000000 | (0x22 << (2 + 2 + 9))
  }
#elif defined(LPC24XX_EMC_K4S561632E)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,
    .config = 0x680,
    .rascas = EMC_DYN_RASCAS_RAS(3) | EMC_DYN_RASCAS_CAS(3, 0),
    .mode = 0xa0000000 | (0x33 << 12)
  }
#elif defined(LPC24XX_EMC_IS42S32800B)
  {
    .chip_select = (volatile lpc_emc_dynamic *) &EMC_DYN_CFG0,

    /* 256MBit, 8Mx32, 4 banks, row = 12, column = 9, RBC */
    .config = 0x4480,

    #if LPC24XX_EMCCLK == 72000000U
      .rascas = EMC_DYN_RASCAS_RAS(3) | EMC_DYN_RASCAS_CAS(3, 0),
      .mode = 0xa0000000 | (0x32 << (2 + 2 + 9))
    #elif LPC24XX_EMCCLK == 60000000U
      .rascas = EMC_DYN_RASCAS_RAS(2) | EMC_DYN_RASCAS_CAS(2, 0),
      .mode = 0xa0000000 | (0x22 << (2 + 2 + 9))
    #else
      #error "unexpected EMCCLK"
    #endif
  }
#endif
};

BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_emc_dynamic_chip_count =
    sizeof(lpc24xx_start_config_emc_dynamic_chip)
      / sizeof(lpc24xx_start_config_emc_dynamic_chip [0]);
