/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief EBI calibration chip-select configuration.
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
#include <bsp/start.h>
#include <bsp.h>

#ifdef MPC55XX_HAS_EBI

BSP_START_TEXT_SECTION const struct EBI_CAL_CS_tag
  mpc55xx_start_config_ebi_cal_cs [] = {
#if defined(MPC55XX_BOARD_MPC5674FEVB)
  /* External SRAM */
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
  },
  /* External Ethernet controller */
  {
    .BR = {
      .B = {
        .BA = 0x3fff8000 >> 15,
        .PS = 0,
        .AD_MUX = 1,
        .BL = 0,
        .WEBS = 0,
        .TBDIP = 0,
        .SETA = 0,
        .BI = 1,
        .V = 1
      }
    },
    .OR = {
      .B = {
        .AM = 0xfff80000 >> 15,
        .SCY = 1,
        .BSCY = 0
      }
    }
  }
#elif defined(MPC55XX_BOARD_MPC5674F_ECU508)
  /* D_CS0 for external SRAM */
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
        .AM = 0xffe00000 >> 15,
        .SCY = 0,
        .BSCY = 0
      }
    }
  },

  /* D_CS1 for Ethernet Controller */
  {
    .BR = {
      .B = {
        .BA = 0x3fff8000 >> 15,
        .PS = 0,
        .AD_MUX = 1,
        .BL = 0,
        .WEBS = 0,
        .TBDIP = 0,
        .SETA = 0,
        .BI = 1,
        .V = 1
      }
    },
    .OR = {
      .B = {
        .AM = 0xffff8000 >> 15,
        .SCY = 1,
        .BSCY = 0
      }
    }
  },

  /* D_CS2 unused */
  {
    .BR = {
      .B = {
        .BA = 0x20000000 >> 15,
        .PS = 0,
        .AD_MUX = 1,
        .BL = 0,
        .WEBS = 0,
        .TBDIP = 0,
        .SETA = 0,
        .BI = 1,
        .V = 0
      }
    },
    .OR = {
      .B = {
        .AM = 0xfff80000 >> 15,
        .SCY = 0,
        .BSCY = 0
      }
    }
  },

  /* D_CS3 for MRAM, ARCNET */
  {
    .BR = {
      .B = {
        .BA = 0x3ff80000 >> 15,
        .PS = 1,
        .AD_MUX = 1,
        .BL = 0,
        .WEBS = 1,
        .TBDIP = 0,
        .SETA = 0,
        .BI = 1,
        .V = 1
    }
  },
    .OR = {
      .B = {
        .AM = 0xfff80000 >> 15,
        .SCY = 1,
        .BSCY = 0
      }
    }
  }
#endif
};

BSP_START_TEXT_SECTION const size_t mpc55xx_start_config_ebi_cal_cs_count [] = {
  sizeof(mpc55xx_start_config_ebi_cal_cs) / sizeof(mpc55xx_start_config_ebi_cal_cs [0])
};

#endif /* MPC55XX_HAS_EBI */
