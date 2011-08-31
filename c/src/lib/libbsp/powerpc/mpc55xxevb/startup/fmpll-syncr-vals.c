/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP startup code.
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

#include <mpc55xx/regs.h>

#if MPC55XX_CHIP_TYPE / 10 == 551 || MPC55XX_CHIP_TYPE / 10 == 567
/*
 * define init values for FMPLL ESYNCRx
 * (used in start.S/fmpll.S)
 */
#define EPREDIV_VAL (MPC55XX_FMPLL_PREDIV-1)
#define EMFD_VAL    (MPC55XX_FMPLL_MFD-16)
#define VCO_CLK_REF (MPC55XX_FMPLL_REF_CLOCK/(EPREDIV_VAL+1))
#define VCO_CLK_OUT (VCO_CLK_REF*(EMFD_VAL+16))
#define ERFD_VAL    ((VCO_CLK_OUT/MPC55XX_FMPLL_CLK_OUT)-1)

const struct fmpll_syncr_vals_t {
  union FMPLL_ESYNCR2_tag esyncr2_temp;
  union FMPLL_ESYNCR2_tag esyncr2_final;
  union FMPLL_ESYNCR1_tag esyncr1_final;
} mpc55xx_fmpll_config =
  {
    { /* esyncr2_temp */
      .B.LOCEN=0,
      .B.LOLRE=0,
      .B.LOCRE=0,
      .B.LOLIRQ=0,
      .B.LOCIRQ=0,
      .B.ERATE=0,
      .B.EDEPTH=0,
      .B.ERFD=ERFD_VAL+2 /* reduce output clock during init */
    },
    { /* esyncr2_final */
      .B.LOCEN=0,
      .B.LOLRE=0,
      .B.LOCRE=0,
      .B.LOLIRQ=0,
      .B.LOCIRQ=0,
      .B.ERATE=0,
#if MPC55XX_CHIP_TYPE / 10 == 567
      .B.CLKCFG_DIS=1,
#endif
      .B.EDEPTH=0,
      .B.ERFD=ERFD_VAL /* nominal output clock after init */
    },
    { /* esyncr1_final */
      .B.CLKCFG=7,
      .B.EPREDIV=EPREDIV_VAL,
      .B.EMFD=EMFD_VAL
    }
  };
#else /* !(MPC55XX_CHIP_TYPE / 10 == 551 || MPC55XX_CHIP_TYPE / 10 == 567) */
const struct fmpll_syncr_vals_t {
  union FMPLL_SYNCR_tag syncr_temp;
  union FMPLL_SYNCR_tag syncr_final;
} mpc55xx_fmpll_config =
  {
    { /* syncr_temp */
      .B.PREDIV=MPC55XX_FMPLL_PREDIV-1,
      .B.MFD=MPC55XX_FMPLL_MFD,
      .B.RFD=2,
      .B.LOCEN=1
    },
    { /* syncr_final */
      .B.PREDIV=MPC55XX_FMPLL_PREDIV-1,
      .B.MFD=MPC55XX_FMPLL_MFD,
      .B.RFD=0,
      .B.LOCEN=1
    }
  };
#endif /* !(MPC55XX_CHIP_TYPE / 10 == 551 || MPC55XX_CHIP_TYPE / 10 == 567) */
