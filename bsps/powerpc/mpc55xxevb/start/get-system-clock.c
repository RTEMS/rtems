/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief System clock calculation.
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

      system_clock = ((reference_clock / (eprediv + 1)) * (emfd + 16))
        / (erfd + 1);
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
