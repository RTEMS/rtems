/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief BSP start EMC dynamic memory configuration.
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
#include <bsp/lpc24xx.h>

/*
 * FIXME: The NXP example code uses different values for the follwing two
 * defines.  In the NXP example code they depend on the EMCCLK.  It is unclear
 * how these values are determined.  The values from the NXP example code do
 * not work.
 */

/* Use command delayed strategy */
#define LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT 0x1

#define LPC24XX_EMCDLYCTL_DEFAULT 0x1112

BSP_START_DATA_SECTION const lpc24xx_emc_dynamic_config
  lpc24xx_start_config_emc_dynamic [] = {
#if defined(LPC24XX_EMC_MT48LC4M16A2)
  /* Dynamic Memory 0: Micron M T48LC 4M16 A2 P 75 IT */
  {
    /* 15.6 us */
    .refresh = LPC24XX_PS_TO_EMCCLK(15600000, 0) / 16,

    .readconfig = LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT,

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
  /* Dynamic Memory 0: ISSI IS42S32800D7 */
  {
    /* 15.6 us */
    .refresh = LPC24XX_PS_TO_EMCCLK(15600000, 0) / 16,

    .readconfig = LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT,

    /* 20ns */
    .trp = LPC24XX_PS_TO_EMCCLK(20000, 1),

    /* 45ns */
    .tras = LPC24XX_PS_TO_EMCCLK(45000, 1),

    /* 70ns (tXSR) */
    .tsrex = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* 20ns (tRCD) */
    .tapr = LPC24XX_PS_TO_EMCCLK(20000, 1),

    /* n clock cycles -> 38.8ns >= 35ns */
    .tdal = LPC24XX_PS_TO_EMCCLK(35000, 0),

    /* 14ns (tDPL) */
    .twr = LPC24XX_PS_TO_EMCCLK(14000, 1),

    /* 67.5ns */
    .trc = LPC24XX_PS_TO_EMCCLK(67500, 1),

    /* 67.5ns (tRC) */
    .trfc = LPC24XX_PS_TO_EMCCLK(67500, 1),

    /* 70ns */
    .txsr = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* 14ns */
    .trrd = LPC24XX_PS_TO_EMCCLK(14000, 1),

    /* 14ns */
    .tmrd = LPC24XX_PS_TO_EMCCLK(14000, 1),

    .emcdlyctl = LPC24XX_EMCDLYCTL_DEFAULT
  }
#elif defined(LPC24XX_EMC_W9825G2JB75I)
  /* Dynamic Memory 0: Winbond W9825G2JB75I */
  {
    /* 15.6 us */
    .refresh = LPC24XX_PS_TO_EMCCLK(15600000, 0) / 16,

    .readconfig = LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT,

    /* 20ns */
    .trp = LPC24XX_PS_TO_EMCCLK(20000, 1),

    /* 45ns */
    .tras = LPC24XX_PS_TO_EMCCLK(45000, 1),

    /* 75ns (tXSR) */
    .tsrex = LPC24XX_PS_TO_EMCCLK(75000, 1),

    /* 20ns (tRCD) */
    .tapr = LPC24XX_PS_TO_EMCCLK(20000, 1),

    /* tWR + tRP -> 2 * tCK + 20ns */
    .tdal = 2 + LPC24XX_PS_TO_EMCCLK(20000, 0),

    /* (n + 1) clock cycles == 2 * tCK */
    .twr = 1,

    /* 65ns */
    .trc = LPC24XX_PS_TO_EMCCLK(65000, 1),

    /* 65ns (tRC) */
    .trfc = LPC24XX_PS_TO_EMCCLK(65000, 1),

    /* 75ns */
    .txsr = LPC24XX_PS_TO_EMCCLK(50000, 1),

    /* (n + 1) clock cycles == 2 * tCK */
    .trrd = 1,

    /* (n + 1) clock cycles == 2 * tCK (tRSC)*/
    .tmrd = 1,

    .emcdlyctl = LPC24XX_EMCDLYCTL_DEFAULT
  }
#elif defined(LPC24XX_EMC_K4S561632E)
  {
    .refresh = 35,
    .readconfig = LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT,
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
  {
    /* 15.6us */
    .refresh = LPC24XX_PS_TO_EMCCLK(15600000, 0) / 16,

    .readconfig = LPC24XX_EMC_DYNAMIC_READCONFIG_DEFAULT,

    /* 20ns */
    .trp = LPC24XX_PS_TO_EMCCLK(20000, 1),

    /* 45ns */
    .tras = LPC24XX_PS_TO_EMCCLK(45000, 1),

    /* 70ns (tRC) */
    .tsrex = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* FIXME */
    .tapr = LPC24XX_PS_TO_EMCCLK(40000, 1),

    /* tWR + tRP -> 2 * tCK + 20ns */
    .tdal = 2 + LPC24XX_PS_TO_EMCCLK(20000, 0),

    /* (n + 1) clock cycles == 2 * tCK */
    .twr = 1,

    /* 70ns */
    .trc = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* 70ns */
    .trfc = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* 70ns (tRC) */
    .txsr = LPC24XX_PS_TO_EMCCLK(70000, 1),

    /* 14ns */
    .trrd = LPC24XX_PS_TO_EMCCLK(14000, 1),

    /* (n + 1) clock cycles == 2 * tCK */
    .tmrd = 1,

    .emcdlyctl = LPC24XX_EMCDLYCTL_DEFAULT
  }
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
  || defined(LPC24XX_EMC_IS42S32800D7) \
  || defined(LPC24XX_EMC_IS42S32800B)
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
#endif
};

BSP_START_DATA_SECTION const size_t
  lpc24xx_start_config_emc_dynamic_chip_count =
    sizeof(lpc24xx_start_config_emc_dynamic_chip)
      / sizeof(lpc24xx_start_config_emc_dynamic_chip [0]);
