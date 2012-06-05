/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Clock and FMPLL configuration.
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

#include <bsp/mpc55xx-config.h>

BSP_START_TEXT_SECTION const mpc55xx_clock_config
  mpc55xx_start_config_clock [1] = { {
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
    #define ERFD_VAL    ((VCO_CLK_OUT/MPC55XX_FMPLL_CLK_OUT)-1)

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
        #if MPC55XX_CHIP_TYPE / 10  == 567
          .CLKCFG_DIS = 1,
        #endif
        .EDEPTH = 0,
        .ERFD = ERFD_VAL /* nominal output clock after init */
      }
    },
    .esyncr1_final = {
      .B = {
        .CLKCFG = 7,
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
