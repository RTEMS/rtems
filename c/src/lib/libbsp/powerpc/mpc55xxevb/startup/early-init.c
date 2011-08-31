/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP early initialization code.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
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

#include <bsp/mpc55xx-config.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp.h>

#include <mpc55xx/mpc55xx.h>

#include <string.h>

#ifdef MPC55XX_BOOTFLAGS
extern uint32_t mpc55xx_bootflag_0 [];
#endif

static void mpc55xx_siu_init(void)
{
  size_t i = 0;

#if defined(MPC55XX_BOARD_GWLCFM)
  SIU.GPDO[122].B.PDO=1; /* make sure USB reset is kept high */
  SIU.GPDO[121].B.PDO=1; /* make sure Ethernet reset is kept high */
  SIU.GPDO[113].B.PDO=1; /* make sure MOST Companion reset is kept high */
#endif

  for (i = 0; i < mpc55xx_siu_pcr_config_count [0]; ++i) {
     const mpc55xx_siu_pcr_config_entry *e = &mpc55xx_siu_pcr_config [i];
     int j = e->index;
     int n = j + e->count;
     uint32_t pcr = e->pcr.R;

     while (j < n) {
       SIU.PCR [j].R = pcr;
       ++j;
     }
  }
}

static void BSP_START_TEXT_SECTION mpc55xx_ebi_chip_select_init(void)
{
  size_t i = 0;

  for (i = 0; i < mpc55xx_ebi_cs_config_count [0]; ++i) {
     EBI.CS [i] = mpc55xx_ebi_cs_config [i];
  }

  for (i = 0; i < mpc55xx_ebi_cal_cs_config_count [0]; ++i) {
     EBI.CAL_CS [i] = mpc55xx_ebi_cal_cs_config [i];
  }
}

static void BSP_START_TEXT_SECTION mpc55xx_ebi_init(void)
{
#if defined(MPC55XX_BOARD_GWLCFM)
	/*
	 * init EBI for Muxed AD bus
	 */
	EBI.MCR.B.DBM = 1;
	EBI.MCR.B.AD_MUX = 1; /* use multiplexed bus */
	EBI.MCR.B.D16_31 = 1; /* use lower AD bus    */

	SIU.ECCR.B.EBDF = 3;  /* use CLK/4 as bus clock */
#elif defined(MPC55XX_BOARD_MPC5674FEVB)
  struct EBI_tag ebi = {
    .MCR = {
      .B = {
        .ACGE = 0,
        .MDIS = 0,
        .D16_31 = 0,
        .AD_MUX = 0,
        .DBM = 0
      }
    }
  };

  EBI.MCR.R = ebi.MCR.R;
#endif
}

static void BSP_START_TEXT_SECTION mpc55xx_mmu_init(void)
{
#ifdef MPC55XX_BOOTFLAGS
  /* If the low bit of bootflag 0 is clear don't change the MMU.  */
  bool do_mmu_init = (mpc55xx_bootflag_0 [0] & 1) != 0;
#else
  bool do_mmu_init = true;
#endif

  if (do_mmu_init) {
    size_t i = 0;

    for (i = 0; i < mpc55xx_mmu_config_count [0]; ++i) {
      const struct MMU_tag *tag = &mpc55xx_mmu_config [i];
      PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, tag->MAS0.R);
      PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS1, tag->MAS1.R);
      PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS2, tag->MAS2.R);
      PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS3, tag->MAS3.R);
      __asm__ volatile ("tlbwe");
    }
  }
}

static void BSP_START_TEXT_SECTION mpc55xx_load_section(
  void *dst,
  const void *src,
  size_t n
)
{
  if (dst != src) {
    memcpy(dst, src, n);
  }
}

void BSP_START_TEXT_SECTION mpc55xx_early_init(void)
{
  mpc55xx_load_section(
    &bsp_section_fast_text_begin [0],
    &bsp_section_fast_text_load_begin [0],
    (size_t) bsp_section_fast_text_size
  );
  mpc55xx_load_section(
    &bsp_section_fast_data_begin [0],
    &bsp_section_fast_data_load_begin [0],
    (size_t) bsp_section_fast_data_size
  );
  mpc55xx_load_section(
    &bsp_section_data_begin [0],
    &bsp_section_data_load_begin [0],
    (size_t) bsp_section_data_size
  );
  mpc55xx_siu_init();
  mpc55xx_ebi_chip_select_init();
  mpc55xx_ebi_init();
  mpc55xx_mmu_init();
}
