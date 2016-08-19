/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <chip.h>
#include <include/board_memories.h>

const struct BOARD_Sdram_Config BOARD_Sdram_Config = {
  .sdramc_tr = 1562,
  .sdramc_cr =
      SDRAMC_CR_NC_COL8      /* 8 column bits */
    | SDRAMC_CR_NR_ROW11     /* 12 row bits (4K) */
    | SDRAMC_CR_CAS_LATENCY3 /* CAS Latency 3 */
    | SDRAMC_CR_NB_BANK2     /* 2 banks */
    | SDRAMC_CR_DBW          /* 16 bit */
    | SDRAMC_CR_TWR(5)
    | SDRAMC_CR_TRC_TRFC(13) /* 63ns   min */
    | SDRAMC_CR_TRP(5)       /* Command period (PRE to ACT) 21 ns min */
    | SDRAMC_CR_TRCD(5)      /* Active Command to R/W Cmd delay time 21ns min */
    | SDRAMC_CR_TRAS(9)      /* Command period (ACT to PRE)  42ns min */
    | SDRAMC_CR_TXSR(15U),   /* Exit self-refresh to active time  70ns Min */
  .sdramc_mdr = SDRAMC_MDR_MD_SDRAM,
  .sdramc_cfr1 = SDRAMC_CFR1_UNAL_SUPPORTED | SDRAMC_CFR1_TMRD(2)
};
