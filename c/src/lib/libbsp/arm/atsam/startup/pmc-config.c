/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <bsp/atsam-clock-config.h>
#include <bspopts.h>
#include <chip.h>

#if ATSAM_MCK == 123000000
/* PLLA/HCLK/MCK clock is set to 492/246/123MHz */
const struct atsam_clock_config atsam_clock_config = {
  .pllar_init = (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x28U) |
      CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U)),
  .mckr_init = (PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK |
      PMC_MCKR_MDIV_PCK_DIV2),
  .mck_freq = 123*1000*1000
};
#elif ATSAM_MCK == 150000000
/* PLLA/HCLK/MCK clock is set to 300/300/150MHz */
const struct atsam_clock_config atsam_clock_config = {
  .pllar_init = (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x18U) |
      CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U)),
  .mckr_init = (PMC_MCKR_PRES_CLK_1 | PMC_MCKR_CSS_PLLA_CLK |
      PMC_MCKR_MDIV_PCK_DIV2),
  .mck_freq = 150*1000*1000
};
#elif ATSAM_MCK == 60000000
/* PLLA/HCLK/MCK clock is set to 60/60/60MHz */
const struct atsam_clock_config atsam_clock_config = {
  .pllar_init = (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x4U) |
      CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U)),
  .mckr_init = (PMC_MCKR_PRES_CLK_1 | PMC_MCKR_CSS_PLLA_CLK |
      PMC_MCKR_MDIV_EQ_PCK),
  .mck_freq = 60*1000*1000
};
#error Unknown ATSAM_MCK.
#endif
