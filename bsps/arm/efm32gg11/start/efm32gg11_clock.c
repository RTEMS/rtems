/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 System Clocks Setup
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#include <bsp.h>
#include <bsp/processor.h>
#include <bsp/efm32gg11_clock.h>
#include <stdlib.h>


#define HFRCO_FREQ_DEFAULT   19000000

#define PASTE2(a,b)          a ## b
#define PASTE3(a,b,c)        a ## b ## c
#define PASTE4(a,b,c,d)      a ## b ## c ## d

#define HFXO_MODE(t)         PASTE2(CMU_HFXOCTRL_MODE_, t)
#define LFXO_MODE(t)         PASTE2(CMU_LFXOCTRL_MODE_, t)


struct efm32gg11_clock_s efm32gg11_clock;


#if EFM32GG11_HFRCO_FREQ > 0
static struct {
  int32_t freq;
  const volatile uint32_t *cal;
} hfrco_calib[] = {
  {4000000, &DEVINFO->HFRCOCAL0},
  {7000000, &DEVINFO->HFRCOCAL3},
  {13000000, &DEVINFO->HFRCOCAL6},
  {16000000, &DEVINFO->HFRCOCAL7},
  {19000000, &DEVINFO->HFRCOCAL8},
  {26000000, &DEVINFO->HFRCOCAL10},
  {32000000, &DEVINFO->HFRCOCAL11},
  {38000000, &DEVINFO->HFRCOCAL12},
  {48000000, &DEVINFO->HFRCOCAL13},
  {56000000, &DEVINFO->HFRCOCAL14},
  {64000000, &DEVINFO->HFRCOCAL15},
  {72000000, &DEVINFO->HFRCOCAL16}
};
#endif


static void ws_config(void)
{
  uint32_t r;

  MSC->LOCK = MSC_LOCK_LOCKKEY_UNLOCK;

  r = MSC->READCTRL & ~_MSC_READCTRL_MODE_MASK;
  if (efm32gg11_clock.hfclk > 54000000)
    r |= MSC_READCTRL_MODE_WS3;
  else if (efm32gg11_clock.hfclk > 36000000)
    r |= MSC_READCTRL_MODE_WS2;
  else if (efm32gg11_clock.hfclk > 18000000)
    r |= MSC_READCTRL_MODE_WS1;
  else
    r |= MSC_READCTRL_MODE_WS0;
  MSC->READCTRL = r;

  r = MSC->CTRL & ~_MSC_CTRL_WAITMODE_MASK;
  if (efm32gg11_clock.hfclk > 50000000)
    r |= MSC_CTRL_WAITMODE_WS1;
  else
    r |= MSC_CTRL_WAITMODE_WS0;
  MSC->CTRL = r;

  r = MSC->RAMCTRL;
  if (efm32gg11_clock.hfclk > 38000000)
    r |= MSC_RAMCTRL_RAMWSEN | MSC_RAMCTRL_RAM1WSEN | MSC_RAMCTRL_RAM2WSEN;
  else
    r &= ~(MSC_RAMCTRL_RAMWSEN | MSC_RAMCTRL_RAM1WSEN | MSC_RAMCTRL_RAM2WSEN);
  MSC->RAMCTRL = r;

  r = CMU->CTRL;
  if (efm32gg11_clock.hfclk > 32000000)
    r |= CMU_CTRL_WSHFLE;
  else
    r &= ~CMU_CTRL_WSHFLE;
  CMU->CTRL = r;
}

static void divisors_config(void)
{
  uint32_t divisor;

  divisor = 1;
  CMU->HFCOREPRESC = (divisor - 1) << _CMU_HFCOREPRESC_PRESC_SHIFT;
  efm32gg11_clock.hfcoreclk = efm32gg11_clock.hfclk / divisor;

  divisor = (efm32gg11_clock.hfclk > 50000000) ? 2 : 1;
  CMU->HFBUSPRESC = (divisor - 1) << _CMU_HFBUSPRESC_PRESC_SHIFT;
  efm32gg11_clock.hfbusclk = efm32gg11_clock.hfclk / divisor;

  divisor = (efm32gg11_clock.hfclk > 50000000) ? 2 : 1;
  CMU->HFPERPRESC = (divisor - 1) << _CMU_HFPERPRESC_PRESC_SHIFT;
  efm32gg11_clock.hfperclk = efm32gg11_clock.hfclk / divisor;

  divisor = 1;
  CMU->HFPERPRESCB = (divisor - 1) << _CMU_HFPERPRESCB_PRESC_SHIFT;
  efm32gg11_clock.hfperbclk = efm32gg11_clock.hfclk / divisor;

  divisor = (efm32gg11_clock.hfclk > 50000000) ? 2 : 1;
  CMU->HFPERPRESCC = (divisor - 1) << _CMU_HFPERPRESCC_PRESC_SHIFT;
  efm32gg11_clock.hfpercclk = efm32gg11_clock.hfclk / divisor;

  divisor = 2;
  CMU->HFEXPPRESC = (divisor - 1) << _CMU_HFEXPPRESC_PRESC_SHIFT;
  efm32gg11_clock.hfexpclk = efm32gg11_clock.hfclk / divisor;
}

void efm32gg11_clock_init(void)
{
  uint32_t hfrco_ctrl;
  int i;

  CMU->CTRL = CMU_CTRL_HFPERCLKEN;

#if EFM32GG11_LFXO_FREQ > 0
  CMU->FREEZE = 0;
  if (!(CMU->STATUS & CMU_STATUS_LFXOENS)) {
    while (CMU->SYNCBUSY & CMU_SYNCBUSY_LFXOBSY)
      ;
    CMU->LFXOCTRL =
      (EFM32GG11_LFXO_TUNING << _CMU_LFXOCTRL_TUNING_SHIFT) |
      LFXO_MODE(EFM32GG11_LFXO_TYPE) |
      (EFM32GG11_LFXO_GAIN << _CMU_LFXOCTRL_GAIN_SHIFT) |
      CMU_LFXOCTRL_HIGHAMPL_DEFAULT |
      CMU_LFXOCTRL_AGC_DEFAULT |
      CMU_LFXOCTRL_CUR_DEFAULT |
      CMU_LFXOCTRL_BUFCUR_DEFAULT |
      CMU_LFXOCTRL_TIMEOUT_DEFAULT;
    while (CMU->SYNCBUSY & CMU_SYNCBUSY_LFXOBSY)
      ;
    CMU->OSCENCMD = CMU_OSCENCMD_LFXOEN;
  }
  EMU->EM4CTRL |= EMU_EM4CTRL_RETAINLFXO;
  CMU->LFACLKSEL = CMU_LFACLKSEL_LFA_LFXO;
  efm32gg11_clock.lfaclk = EFM32GG11_LFXO_FREQ;
  CMU->LFBCLKSEL = CMU_LFBCLKSEL_LFB_LFXO;
  efm32gg11_clock.lfbclk = EFM32GG11_LFXO_FREQ;
  CMU->LFCCLKSEL = CMU_LFCCLKSEL_LFC_LFXO;
  efm32gg11_clock.lfcclk = EFM32GG11_LFXO_FREQ;
#else
  CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
  CMU->LFACLKSEL = CMU_LFACLKSEL_LFA_LFRCO;
  efm32gg11_clock.lfaclk = 32768;
  CMU->LFBCLKSEL = CMU_LFBCLKSEL_LFB_LFRCO;
  efm32gg11_clock.lfbclk = 32768;
  CMU->LFCCLKSEL = CMU_LFCCLKSEL_LFC_LFRCO;
  efm32gg11_clock.lfcclk = 32768;
#endif

  CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_ULFRCO;
  efm32gg11_clock.lfeclk = 1000;

  efm32gg11_clock.hfclk = HFRCO_FREQ_DEFAULT;

#if EFM32GG11_HFXO_FREQ > 0
  efm32gg11_clock.hfclk = EFM32GG11_HFXO_FREQ;
  CMU->HFXOCTRL = CMU_HFXOCTRL_AUTOSTARTSELEM0EM1_DEFAULT |
                  CMU_HFXOCTRL_AUTOSTARTEM0EM1_DEFAULT |
                  CMU_HFXOCTRL_LFTIMEOUT_DEFAULT |
                  CMU_HFXOCTRL_PEAKDETMODE_DEFAULT |
#if EFM32GG11_HFXO_FREQ <= 25000000
                  CMU_HFXOCTRL_HFXOX2EN |
#endif
                  HFXO_MODE(EFM32GG11_HFXO_TYPE);
  CMU->HFXOSTARTUPCTRL =
    EFM32GG11_HFXO_TUNING << _CMU_HFXOSTARTUPCTRL_CTUNE_SHIFT |
    CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT;
  CMU->HFXOSTEADYSTATECTRL =
    EFM32GG11_HFXO_TUNING << _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT |
    CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT;
  CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN;
#endif

#if EFM32GG11_HFRCO_FREQ > 0
  for (i = 0; i < sizeof(hfrco_calib) / sizeof(hfrco_calib[0]) - 1; i++) {
    if (abs(hfrco_calib[i + 1].freq - EFM32GG11_HFRCO_FREQ) >
        abs(hfrco_calib[i].freq - EFM32GG11_HFRCO_FREQ))
      break;
  }
  efm32gg11_clock.hfclk = hfrco_calib[i].freq;
  hfrco_ctrl = *hfrco_calib[i].cal;
#else
  (void) i;
  (void) hfrco_ctrl;
#endif

  if (efm32gg11_clock.hfclk > HFRCO_FREQ_DEFAULT) {
    ws_config();
    divisors_config();
  }

#if EFM32GG11_HFRCO_FREQ > 0
  CMU->HFRCOCTRL = hfrco_ctrl;
#elif EFM32GG11_HFXO_FREQ > 0
  while (!(CMU->STATUS & CMU_STATUS_HFXORDY))
    ;
  CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFXO;
  CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
#endif

  if (efm32gg11_clock.hfclk <= HFRCO_FREQ_DEFAULT) {
    ws_config();
    divisors_config();
  }
}
