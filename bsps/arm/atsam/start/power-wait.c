/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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

#include <bsp/power.h>
#include <bsp/linker-symbols.h>

#include <libchip/chip.h>

BSP_FAST_TEXT_SECTION static void
pmc_wait_for_master_clock_ready(volatile Pmc *pmc)
{
  while ((pmc->PMC_SR & PMC_SR_MCKRDY) == 0) {
    /* Wait */
  }
}

BSP_FAST_TEXT_SECTION static void
pmc_set_master_clock_source(volatile Pmc *pmc, uint32_t mckr)
{
  pmc->PMC_MCKR =
    (pmc->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | (mckr & PMC_MCKR_CSS_Msk);
}

BSP_FAST_TEXT_SECTION static void
pmc_set_master_clock_prescaler(volatile Pmc *pmc, uint32_t mckr)
{
  pmc->PMC_MCKR =
    (pmc->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | (mckr & PMC_MCKR_PRES_Msk);
}

BSP_FAST_TEXT_SECTION static void
pmc_set_master_clock_division(volatile Pmc *pmc, uint32_t mckr)
{
  pmc->PMC_MCKR =
    (pmc->PMC_MCKR & ~PMC_MCKR_MDIV_Msk) | (mckr & PMC_MCKR_MDIV_Msk);
}

BSP_FAST_TEXT_SECTION static void
pmc_wait_for_main_rc_osc(volatile Pmc *pmc)
{
  while ((pmc->PMC_SR & PMC_SR_MOSCRCS) == 0) {
    /* Wait */
  }
}

BSP_FAST_TEXT_SECTION static void
pmc_wait_for_main_osc_selection(volatile Pmc *pmc)
{
  while ((pmc->PMC_SR & PMC_SR_MOSCSELS) == 0) {
    /* Wait */
  }
}

BSP_FAST_TEXT_SECTION static void
pmc_use_main_rc_osc_4mhz(volatile Pmc *pmc)
{
  uint32_t ckgr_mor;

  ckgr_mor = pmc->CKGR_MOR;
  ckgr_mor |= CKGR_MOR_KEY_PASSWD;

  /* Enable main RC oscillator */
  ckgr_mor |= CKGR_MOR_MOSCRCEN;
  PMC->CKGR_MOR = ckgr_mor;
  pmc_wait_for_main_rc_osc(pmc);

  /* Set main RC oscillator frequency to 4MHz */
  ckgr_mor &= ~CKGR_MOR_MOSCRCF_Msk;
  ckgr_mor |= CKGR_MOR_MOSCRCF_4_MHz;
  pmc->CKGR_MOR = ckgr_mor;
  pmc_wait_for_main_rc_osc(pmc);

  /* Switch to main RC oscillator */
  ckgr_mor &= ~CKGR_MOR_MOSCSEL;
  pmc->CKGR_MOR = ckgr_mor;
  pmc_wait_for_main_osc_selection(pmc);
  pmc_wait_for_master_clock_ready(pmc);
}

BSP_FAST_TEXT_SECTION static void
pmc_use_main_ext_osc(volatile Pmc *pmc)
{
  uint32_t ckgr_mor;

  ckgr_mor = pmc->CKGR_MOR;
  ckgr_mor |= CKGR_MOR_KEY_PASSWD;

  /* Enable main external oscillator */
  ckgr_mor |= CKGR_MOR_MOSCRCF_12_MHz | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCRCEN |
    CKGR_MOR_MOSCXTST(DEFAUTL_MAIN_OSC_COUNT);
  PMC->CKGR_MOR = ckgr_mor;
  pmc_wait_for_main_rc_osc(pmc);
  pmc_wait_for_master_clock_ready(pmc);

  /* Switch to main external oscillator */
  ckgr_mor |= CKGR_MOR_MOSCSEL;
  pmc->CKGR_MOR = ckgr_mor;
  pmc_wait_for_main_osc_selection(pmc);
  pmc_wait_for_master_clock_ready(pmc);
}

BSP_FAST_TEXT_SECTION void
atsam_power_handler_wait_mode(
  const atsam_power_control *control,
  atsam_power_state state
)
{
  rtems_interrupt_level level;
  volatile Pmc *pmc;
  uint32_t mckr;
  uint32_t fmr;
  uint32_t fsmr;

  (void) control;
  pmc = PMC;

  switch (state) {
    case ATSAM_POWER_OFF:
      rtems_interrupt_disable(level);

      mckr = pmc->PMC_MCKR;

      /* Switch main clock to main RC oscillator (4MHz) */
      pmc_use_main_rc_osc_4mhz(pmc);
      pmc_set_master_clock_source(pmc, PMC_MCKR_CSS_MAIN_CLK);
      pmc_wait_for_master_clock_ready(pmc);
      pmc_set_master_clock_prescaler(pmc, PMC_MCKR_PRES_CLK_1);
      pmc_wait_for_master_clock_ready(pmc);
      pmc_set_master_clock_division(pmc, PMC_MCKR_MDIV_EQ_PCK);
      pmc_wait_for_master_clock_ready(pmc);

      /* Set flash wait state to 0 */
      fmr = EFC->EEFC_FMR;
      EFC->EEFC_FMR = EEFC_FMR_FWS(0);

      /* Enter wait mode */
      fsmr = pmc->PMC_FSMR;
      pmc->PMC_FSMR = fsmr | PMC_FSMR_LPM;
      pmc->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_WAITMODE;
      pmc_wait_for_master_clock_ready(pmc);
      __asm__ volatile ( "wfe" : : : "memory" );
      pmc->PMC_FSMR = fsmr;

      /* Restore main clock */
      pmc_set_master_clock_prescaler(pmc, mckr);
      pmc_wait_for_master_clock_ready(pmc);
      pmc_set_master_clock_division(pmc, mckr);
      pmc_wait_for_master_clock_ready(pmc);
      pmc_set_master_clock_source(pmc, mckr);
      pmc_wait_for_master_clock_ready(pmc);
      pmc_use_main_ext_osc(pmc);

      /* Restore flash wait state */
      EFC->EEFC_FMR = fmr;

      rtems_interrupt_enable(level);
      break;
    case ATSAM_POWER_INIT:
      rtems_interrupt_disable(level);

      pmc->PMC_FSMR = (pmc->PMC_FSMR & ~PMC_FSMR_FLPM_Msk)
        | PMC_FSMR_FLPM(PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN);

      /*  No Cortex-M7 Deep Sleep mode (Backup Mode) */
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

      rtems_interrupt_enable(level);
    default:
      break;
  }
}
