/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief System clock calculation.
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
#include <bsp/mpc55xx-config.h>

uint32_t mpc55xx_get_system_clock(void)
{
  uint32_t system_clock = 0;

  #ifdef MPC55XX_HAS_FMPLL
    volatile struct FMPLL_tag *fmpll = &FMPLL;
    union FMPLL_SYNSR_tag synsr = { .R = fmpll->SYNSR.R };
    uint32_t reference_clock = MPC55XX_REFERENCE_CLOCK;
    bool pll_clock_mode = synsr.B.MODE != 0;
    bool crystal_or_external_reference_mode = synsr.B.PLLSEL != 0;

    if (pll_clock_mode) {
      if (crystal_or_external_reference_mode) {
        union FMPLL_SYNCR_tag syncr = { .R = fmpll->SYNCR.R };
        uint32_t prediv = syncr.B.PREDIV;
        uint32_t mfd = syncr.B.MFD;
        uint32_t rfd = syncr.B.RFD;

        system_clock = ((reference_clock * (mfd + 4)) >> rfd) / (prediv + 1);
      } else {
        system_clock = 2 * reference_clock;
      }
    } else {
      system_clock = reference_clock;
    }
  #endif

  #ifdef MPC55XX_HAS_FMPLL_ENHANCED
    volatile struct FMPLL_tag *fmpll = &FMPLL;
    union FMPLL_ESYNCR1_tag esyncr1 = { .R = fmpll->ESYNCR1.R };
    uint32_t reference_clock = MPC55XX_REFERENCE_CLOCK;
    bool normal_mode = (esyncr1.B.CLKCFG & 0x4U) != 0;

    if (normal_mode) {
      union FMPLL_ESYNCR2_tag esyncr2 = { .R = fmpll->ESYNCR2.R };
      uint32_t eprediv = esyncr1.B.EPREDIV;
      uint32_t emfd = esyncr1.B.EMFD;
      uint32_t erfd = esyncr2.B.ERFD;

      system_clock = (reference_clock * (emfd + 16))
        / ((erfd + 1) * (eprediv + 1));
    } else {
      system_clock = reference_clock;
    }
  #endif

  #ifdef MPC55XX_HAS_MODE_CONTROL
    /* FIXME: Assumes normal mode and external oscillator */
    PLLD_CR_32B_tag cr = { . R = CGM.FMPLL [0].CR.R };
    uint32_t xosc = MPC55XX_REFERENCE_CLOCK;
    uint32_t ldf = cr.B.NDIV;
    uint32_t idf = cr.B.IDF + 1;
    uint32_t odf = 2U << cr.B.ODF;

    system_clock = (xosc * ldf) / (idf * odf);
  #endif

  return system_clock;
}
