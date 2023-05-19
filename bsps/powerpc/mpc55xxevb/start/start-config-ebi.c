/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief EBI configuration.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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
