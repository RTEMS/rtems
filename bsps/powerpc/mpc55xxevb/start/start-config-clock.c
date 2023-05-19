/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Clock and FMPLL configuration.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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

const mpc55xx_clock_config mpc55xx_start_config_clock [1] = { {
  #ifdef MPC55XX_HAS_FMPLL
    .syncr_tmp = {
      .B = {
        .PREDIV = MPC55XX_FMPLL_PREDIV - 1,
        .MFD = MPC55XX_FMPLL_MFD,
        .RFD = 2,
        .LOCEN = 1
      }
    },
    .syncr_final = {
      .B = {
        .PREDIV = MPC55XX_FMPLL_PREDIV - 1,
        .MFD = MPC55XX_FMPLL_MFD,
        .RFD = 0,
        .LOCEN = 1,
        .LOLIRQ = 1,
        .LOCIRQ = 1
      }
    }
  #endif
  #ifdef MPC55XX_HAS_FMPLL_ENHANCED
    #define EPREDIV_VAL (MPC55XX_FMPLL_PREDIV-1)
    #define EMFD_VAL    (MPC55XX_FMPLL_MFD-16)
    #define VCO_CLK_REF (MPC55XX_REFERENCE_CLOCK/(EPREDIV_VAL+1))
    #define VCO_CLK_OUT (VCO_CLK_REF*(EMFD_VAL+16))
    #define ERFD_VAL \
      (((VCO_CLK_OUT + MPC55XX_SYSTEM_CLOCK - 1) / MPC55XX_SYSTEM_CLOCK)-1)

    .esyncr2_tmp = {
      .B = {
        .LOCEN = 0,
        .LOLRE = 0,
        .LOCRE = 0,
        .LOLIRQ = 0,
        .LOCIRQ = 0,
        .ERATE = 0,
        .EDEPTH = 0,
        .ERFD = ERFD_VAL + 2 /* reduce output clock during init */
      }
    },
    .esyncr2_final = {
      .B = {
        .LOCEN = 0,
        .LOLRE = 0,
        .LOCRE = 0,
        .LOLIRQ = 0,
        .LOCIRQ = 0,
        .ERATE = 0,
        #if MPC55XX_CHIP_FAMILY  == 567
          .CLKCFG_DIS = 1,
        #endif
        .EDEPTH = 0,
        .ERFD = ERFD_VAL /* nominal output clock after init */
      }
    },
    .esyncr1_final = {
      .B = {
        .CLKCFG = MPC55XX_FMPLL_ESYNCR1_CLKCFG,
        .EPREDIV = EPREDIV_VAL,
        .EMFD = EMFD_VAL
      }
    }
  #endif
  #ifdef MPC55XX_HAS_MODE_CONTROL
    .fmpll = {
      {
        .cr = {
          #if MPC55XX_REFERENCE_CLOCK == 8000000
            .B = { .IDF = 0, .ODF = 1, .NDIV = 60, .I_LOCK = 1, .PLL_ON = 1 }
          #elif MPC55XX_REFERENCE_CLOCK == 40000000
            .B = { .IDF = 3, .ODF = 1, .NDIV = 48, .I_LOCK = 1, .PLL_ON = 1 }
          #else
            #error "unexpected reference clock"
          #endif
        }
      },
      {
        .cr = {
          .B = { .IDF = 3, .ODF = 2, .NDIV = 32, .I_LOCK = 1, .PLL_ON = 1 }
        }
      }
    },
    .ocds_sc = {
      .B = { .SELDIV = 2, .SELCTL = 2 }
    },
    .auxclk = {
      [0] = {
        .AC_SC = { .B = { .SELCTL = 4 } },
        .AC_DC0_3 = { .B = { .DE0 = 1, .DIV0 = 0 } }
      },
      [1] = {
        .AC_SC = { .B = { .SELCTL = 4 } },
        .AC_DC0_3 = { .B = { .DE0 = 1, .DIV0 = 11 } }
      },
      [2] = {
        .AC_SC = { .B = { .SELCTL = 4 } },
        .AC_DC0_3 = { .B = { .DE0 = 1, .DIV0 = 11 } }
      },
      [3] = {
        .AC_SC = { .B = { .SELCTL = 1 } }
      },
      [4] = {
        .AC_SC = { .B = { .SELCTL = 1 } }
      }
    }
  #endif
} };
