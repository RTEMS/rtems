/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MPC55XX EBI calibration chip-select configuration.
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
#include <bsp/start.h>
#include <bsp.h>

const BSP_START_TEXT_SECTION struct EBI_CAL_CS_tag
mpc55xx_ebi_cal_cs_config [] = {
#if defined(MPC55XX_BOARD_MPC5674FEVB)
  {
    .BR = {
      .B = {
        .BA = 0x20000000 >> 15,
        .PS = 0,
        .AD_MUX = 1,
        .BL = 1,
        .WEBS = 0,
        .TBDIP = 1,
        .SETA = 0,
        .BI = 0,
        .V = 1
      }
    },
    .OR = {
      .B = {
        .AM = 0xfff80000 >> 15,
        .SCY = 0,
        .BSCY = 0
      }
    }
  }
#endif
};

const BSP_START_TEXT_SECTION size_t mpc55xx_ebi_cal_cs_config_count [] = {
  sizeof(mpc55xx_ebi_cal_cs_config) / sizeof(mpc55xx_ebi_cal_cs_config [0])
};
