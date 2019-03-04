/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief EBI configuration.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/mpc55xx-config.h>

#ifdef MPC55XX_HAS_EBI

const mpc55xx_ebi_config mpc55xx_start_config_ebi [] = {
  #if defined(MPC55XX_BOARD_GWLCFM)
    {
      .ebi_mcr = {
        .B = {
          .DBM = 1,
          .AD_MUX = 1, /* use multiplexed bus */
          .D16_31 = 1 /* use lower AD bus    */
        }
      },
      .siu_eccr_ebdf = 4 - 1 /* use CLK/4 as bus clock */
    }
  #elif (defined(MPC55XX_BOARD_MPC5674FEVB) \
    || defined(MPC55XX_BOARD_MPC5674F_ECU508) \
    || defined(MPC55XX_BOARD_MPC5674F_RSM6)) \
      && defined(MPC55XX_NEEDS_LOW_LEVEL_INIT)
    {
      .ebi_mcr = {
        .B = {
          .ACGE = 0,
          .MDIS = 0,
          .D16_31 = 1,
          .AD_MUX = 0,
          .DBM = 0
        }
      },
      .siu_eccr_ebdf = 2 - 1
    }
  #endif
};

const size_t mpc55xx_start_config_ebi_count [] = {
  RTEMS_ARRAY_SIZE(mpc55xx_start_config_ebi)
};

#endif /* MPC55XX_HAS_EBI */
