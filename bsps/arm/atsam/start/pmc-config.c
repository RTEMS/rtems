/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
