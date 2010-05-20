/**
 * @file
 *
 * @ingroup lpc24xx_clocks
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/utility.h>
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
  unsigned end = start + us * (lpc24xx_cclk() / 1000000);
  unsigned now = 0;

  do {
    now = lpc24xx_timer();
  } while (now < end);
}

unsigned lpc24xx_pllclk(void)
{
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

  return pllclk;
}

unsigned lpc24xx_cclk(void)
{
  /* Get PLL output frequency */
  unsigned pllclk = lpc24xx_pllclk();

  /* Get CPU frequency */
  unsigned cclk = pllclk / (GET_CCLKCFG_CCLKSEL(CCLKCFG) + 1);

  return cclk;
}
