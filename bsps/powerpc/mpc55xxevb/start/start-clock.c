/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Clock and FMPLL initialization code.
 */

/*
 * Copyright (C) 2008, 2011 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/fatal.h>
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
        bsp_fatal(MPC55XX_FATAL_FMPLL_LOCK);
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

      #if MPC55XX_CHIP_FAMILY == 551 || MPC55XX_CHIP_FAMILY == 566
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
