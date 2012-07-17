/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Clock and FMPLL initialization code.
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
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/bootcard.h>
#include <bsp/mpc55xx-config.h>

#ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
  #if defined(MPC55XX_HAS_FMPLL) || defined(MPC55XX_HAS_FMPLL_ENHANCED)
    static BSP_START_TEXT_SECTION void fmpll_wait_for_lock(void)
    {
      int i = 0;
      bool lock = false;

      while (!lock && i < 6000) {
        lock = FMPLL.SYNSR.B.LOCK != 0;
        ++i;
      }

      if (!lock) {
        bsp_reset();
      }
    }
  #endif
#endif

BSP_START_TEXT_SECTION void mpc55xx_start_clock(void)
{
  #ifdef MPC55XX_NEEDS_LOW_LEVEL_INIT
    const mpc55xx_clock_config *cfg = mpc55xx_start_config_clock;

    #ifdef MPC55XX_HAS_FMPLL
      volatile struct FMPLL_tag *fmpll = &FMPLL;

      fmpll->SYNCR.R = cfg->syncr_tmp.R;
      fmpll->SYNCR.R;
      fmpll_wait_for_lock();

      fmpll->SYNCR.R = cfg->syncr_final.R;
      fmpll->SYNCR.R;
      fmpll_wait_for_lock();
    #endif

    #ifdef MPC55XX_HAS_FMPLL_ENHANCED
      volatile struct FMPLL_tag *fmpll = &FMPLL;

      fmpll->ESYNCR2.R = cfg->esyncr2_tmp.R;
      fmpll->ESYNCR2.R;
      fmpll->ESYNCR1.R = cfg->esyncr1_final.R;
      fmpll->ESYNCR1.R;
      fmpll_wait_for_lock();

      fmpll->ESYNCR2.R = cfg->esyncr2_final.R;
      fmpll->ESYNCR2.R;
      fmpll_wait_for_lock();

      #if MPC55XX_CHIP_TYPE / 10 == 551
        /* System clock supplied by PLL */
        SIU.SYSCLK.B.SYSCLKSEL = 2;
      #endif
    #endif

    #ifdef MPC55XX_HAS_MODE_CONTROL
      volatile CGM_tag *cgm = &CGM;
      size_t fmpll_count = sizeof(cfg->fmpll) / sizeof(cfg->fmpll [0]);
      size_t auxclk_count = sizeof(cfg->auxclk) / sizeof(cfg->auxclk [0]);
      size_t i = 0;

      for (i = 0; i < auxclk_count; ++i) {
        cgm->AUXCLK [i].AC_SC.R = cfg->auxclk [i].AC_SC.R;
        cgm->AUXCLK [i].AC_DC0_3.R = cfg->auxclk [i].AC_DC0_3.R;
      }

      for (i = 0; i < fmpll_count; ++i) {
        cgm->FMPLL [i].CR.R = cfg->fmpll [i].cr.R;
        cgm->FMPLL [i].MR.R = cfg->fmpll [i].mr.R;
      }

      cgm->OC_EN.R = cfg->oc_en.R;
      cgm->OCDS_SC.R = cfg->ocds_sc.R;
    #endif
  #endif
}
