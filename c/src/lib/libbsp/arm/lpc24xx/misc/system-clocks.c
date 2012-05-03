/**
 * @file
 *
 * @ingroup lpc24xx_clocks
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/lpc24xx.h>
#include <bsp/system-clocks.h>

/**
 * @brief Internal RC oscillator frequency in [Hz].
 */
#define LPC24XX_OSCILLATOR_INTERNAL 4000000U

#ifndef LPC24XX_OSCILLATOR_MAIN
  #error "unknown main oscillator frequency"
#endif

#ifndef LPC24XX_OSCILLATOR_RTC
  #error "unknown RTC oscillator frequency"
#endif

void lpc24xx_timer_initialize(void)
{
  /* Reset timer */
  T1TCR = TCR_RST;

  /* Set timer mode */
  T1CTCR = 0;

  /* Set prescaler to zero */
  T1PR = 0;

  /* Reset all interrupt flags */
  T1IR = 0xff;

  /* Do not stop on a match */
  T1MCR = 0;

  /* No captures */
  T1CCR = 0;

  /* Start timer */
  T1TCR = TCR_EN;
}

void lpc24xx_micro_seconds_delay(unsigned us)
{
  unsigned start = lpc24xx_timer();
  unsigned delay = us * (LPC24XX_PCLK / 1000000);
  unsigned elapsed = 0;

  do {
    elapsed = lpc24xx_timer() - start;
  } while (elapsed < delay);
}

#ifdef ARM_MULTILIB_ARCH_V7M
  unsigned lpc17xx_sysclk(unsigned clksrcsel)
  {
    return (clksrcsel & LPC17XX_SCB_CLKSRCSEL_CLKSRC) != 0 ?
      LPC24XX_OSCILLATOR_MAIN
        : LPC24XX_OSCILLATOR_INTERNAL;
  }
#endif

unsigned lpc24xx_pllclk(void)
{
  #ifdef ARM_MULTILIB_ARCH_V4
    unsigned clksrc = GET_CLKSRCSEL_CLKSRC(CLKSRCSEL);
    unsigned pllinclk = 0;
    unsigned pllclk = 0;

    /* Get PLL input frequency */
    switch (clksrc) {
      case 0:
        pllinclk = LPC24XX_OSCILLATOR_INTERNAL;
        break;
      case 1:
        pllinclk = LPC24XX_OSCILLATOR_MAIN;
        break;
      case 2:
        pllinclk = LPC24XX_OSCILLATOR_RTC;
        break;
      default:
        return 0;
    }

    /* Get PLL output frequency */
    if ((PLLSTAT & PLLSTAT_PLLC) != 0) {
      uint32_t pllcfg = PLLCFG;
      unsigned n = GET_PLLCFG_NSEL(pllcfg) + 1;
      unsigned m = GET_PLLCFG_MSEL(pllcfg) + 1;

      pllclk = (pllinclk / n) * 2 * m;
    } else {
      pllclk = pllinclk;
    }
  #else
    volatile lpc17xx_scb *scb = &LPC17XX_SCB;
    unsigned sysclk = lpc17xx_sysclk(scb->clksrcsel);
    unsigned pllstat = scb->pll_0.stat;
    unsigned pllclk = 0;
    unsigned enabled_and_locked = LPC17XX_PLL_STAT_PLLE
      | LPC17XX_PLL_STAT_PLOCK;

    if ((pllstat & enabled_and_locked) == enabled_and_locked) {
      unsigned m = LPC17XX_PLL_SEL_MSEL_GET(pllstat) + 1;

      pllclk = sysclk * m;
    }
  #endif

  return pllclk;
}

unsigned lpc24xx_cclk(void)
{
  #ifdef ARM_MULTILIB_ARCH_V4
    /* Get PLL output frequency */
    unsigned pllclk = lpc24xx_pllclk();

    /* Get CPU frequency */
    unsigned cclk = pllclk / (GET_CCLKCFG_CCLKSEL(CCLKCFG) + 1);
  #else
    volatile lpc17xx_scb *scb = &LPC17XX_SCB;
    unsigned cclksel = scb->cclksel;
    unsigned cclk_in = 0;
    unsigned cclk = 0;

    if ((cclksel & LPC17XX_SCB_CCLKSEL_CCLKSEL) != 0) {
      cclk_in = lpc24xx_pllclk();
    } else {
      cclk_in = lpc17xx_sysclk(scb->clksrcsel);
    }

    cclk = cclk_in / LPC17XX_SCB_CCLKSEL_CCLKDIV_GET(cclksel);
  #endif

  return cclk;
}
