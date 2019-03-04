/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_clocks
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/system-clocks.h>

/**
 * @brief Internal RC oscillator frequency in [Hz].
 */
#define LPC176X_OSCILLATOR_INTERNAL 4000000u

#ifndef LPC176X_OSCILLATOR_MAIN
#error "unknown main oscillator frequency"
#endif

#ifndef LPC176X_OSCILLATOR_RTC
#error "unknown RTC oscillator frequency"
#endif

inline unsigned lpc176x_sysclk( void );

void lpc176x_timer_initialize( void )
{
  /* Reset timer */
  LPC176X_T1TCR = TCR_RST;
  /* Set timer mode */
  LPC176X_T1CTCR = 0u;
  /* Set prescaler to zero */
  LPC176X_T1PR = 0u;
  /* Reset all interrupt flags */
  LPC176X_T1IR = 0xffU;
  /* Do not stop on a match */
  LPC176X_T1MCR = 0u;
  /* No captures */
  LPC176X_T1CCR = 0u;
  /* Start timer */
  LPC176X_T1TCR = TCR_EN;
}

void lpc176x_micro_seconds_delay( const unsigned us )
{
  const unsigned start = lpc176x_get_timer1();
  const unsigned delay = us * ( LPC176X_PCLK / 1000000u );
  unsigned       elapsed = 0u;

  do {
    elapsed = lpc176x_get_timer1() - start;
  } while ( elapsed < delay );
}

unsigned lpc176x_sysclk( void )
{
  return ( LPC176X_SCB.clksrcsel & LPC176X_SCB_CLKSRCSEL_CLKSRC ) != 0u ?
         LPC176X_OSCILLATOR_MAIN : LPC176X_OSCILLATOR_INTERNAL;
}

unsigned lpc176x_pllclk( void )
{
  const unsigned sysclk = lpc176x_sysclk();
  const unsigned pllstat = ( LPC176X_SCB.pll_0 ).stat;
  const unsigned enabled_and_locked = LPC176X_PLL_STAT_PLLE |
                                      LPC176X_PLL_STAT_PLOCK;
  unsigned pllclk = 0u;

  if ( ( pllstat & enabled_and_locked ) == enabled_and_locked ) {
    unsigned m = LPC176X_PLL_SEL_MSEL_GET( pllstat ) + 1u;
    pllclk = sysclk * m;
  }

  /* else implies that the pllstat is unlocked. Also,
     there is nothing to do. */

  return pllclk;
}

unsigned lpc176x_cclk( void )
{
  const unsigned cclksel = LPC176X_SCB.cclksel;
  unsigned       cclk_in = 0u;
  unsigned       cclk = 0u;

  if ( ( cclksel & LPC176X_SCB_CCLKSEL_CCLKSEL ) != 0u ) {
    cclk_in = lpc176x_pllclk();
  } else {
    cclk_in = lpc176x_sysclk();
  }

  cclk = cclk_in / LPC176X_SCB_CCLKSEL_CCLKDIV_GET( cclksel );

  return cclk;
}

uint32_t _CPU_Counter_frequency(void)
{
  return LPC176X_PCLK;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  return lpc176x_get_timer1();
}
