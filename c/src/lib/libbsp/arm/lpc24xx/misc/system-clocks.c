/**
 * @file
 *
 * @ingroup lpc24xx
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

#ifdef LPC2478
  /**
   * @brief Main oscillator frequency in [Hz].
   */
  #define LPC24XX_OSCILLATOR_MAIN 12000000U

  /**
   * @brief RTC oscillator frequency in [Hz].
   */
  #define LPC24XX_OSCILLATOR_RTC 32768
#else
  #error Unknown oscillator frequencies
#endif

/**
 * @brief Returns the CPU clock frequency in [Hz].
 */
unsigned lpc24xx_cclk( void)
{
  unsigned clksrc = GET_CLKSRCSEL_CLKSRC( CLKSRCSEL);
  unsigned pllinclk = 0;
  unsigned pllclk = 0;
  unsigned cclk = 0;

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
      while (1) {
        /* Spin forever */
      }
      return 0;
  }

  /* Get PLL output frequency */
  if (REG_FLAG_IS_SET( PLLSTAT, PLLSTAT_PLLC)) {
    uint32_t pllcfg = PLLCFG;
    unsigned n = GET_PLLCFG_NSEL( pllcfg) + 1;
    unsigned m = GET_PLLCFG_MSEL( pllcfg) + 1;

    pllclk = (pllinclk / n) * 2 * m;
  } else {
    pllclk = pllinclk;
  }

  /* Get CPU clock frequency */
  cclk = pllclk / (GET_CCLKCFG_CCLKSEL( CCLKCFG) + 1);

  return cclk;
}

static void lpc24xx_pll_config( uint32_t val)
{
  PLLCON = val;
  PLLFEED = 0xaa;
  PLLFEED = 0x55;
}

/**
 * @brief Sets the Phase Locked Loop (PLL).
 *
 * @param clksrc Selects the clock source for the PLL.
 *
 * @param nsel Selects PLL pre-divider value (sometimes named psel).
 *
 * @param msel Selects PLL multiplier value.
 *
 * @param cclksel Selects the divide value for creating the CPU clock (CCLK)
 * from the PLL output.
 *
 * @note All parameter values are the actual register field values.
 */
void lpc24xx_set_pll( unsigned clksrc, unsigned nsel, unsigned msel, unsigned cclksel)
{
  bool pll_enabled = REG_FLAG_IS_SET( PLLSTAT, PLLSTAT_PLLE);

  /* Disconnect PLL if necessary */
  if (REG_FLAG_IS_SET( PLLSTAT, PLLSTAT_PLLC)) {
    if (pll_enabled) {
      lpc24xx_pll_config( PLLCON_PLLE);
    } else {
      lpc24xx_pll_config( 0);
    }
  }

  /* Set CPU clock divider to a reasonable save value */
  CCLKCFG = SET_CCLKCFG_CCLKSEL( 0, 1);

  /* Disable PLL if necessary */
  if (pll_enabled) {
    lpc24xx_pll_config( 0);
  }

  /* Select clock source */
  CLKSRCSEL = SET_CLKSRCSEL_CLKSRC( 0, clksrc);

  /* Set PLL Configuration Register */
  PLLCFG = SET_PLLCFG_NSEL( 0, nsel) | SET_PLLCFG_MSEL( 0, msel);

  /* Enable PLL */
  lpc24xx_pll_config( PLLCON_PLLE);

  /* Wait for lock */
  while (REG_FLAG_IS_CLEARED( PLLSTAT, PLLSTAT_PLOCK)) {
    /* Wait */
  }

  /* Set CPU clock divider and ensure that we have an odd value */
  CCLKCFG = SET_CCLKCFG_CCLKSEL( 0, cclksel | 1);

  /* Connect PLL */
  lpc24xx_pll_config( PLLCON_PLLE | PLLCON_PLLC);
}
