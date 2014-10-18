/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <assert.h>
#include <bsp/stm32f4.h>

#ifdef STM32F4_FAMILY_F4XXXX

#include <bsp/stm32f4xxxx_rcc.h>
#include <bsp/stm32f4xxxx_flash.h>

static rtems_status_code set_system_clk(
  uint32_t sys_clk,
  uint32_t hse_clk,
  uint32_t hse_flag
);

static void init_main_osc( void )
{
  volatile stm32f4_rcc *rcc = STM32F4_RCC;
  rtems_status_code     status;

  /* Revert to reset values */
  rcc->cr |= RCC_CR_HSION;   /* turn on HSI */

  while ( !( rcc->cr & RCC_CR_HSIRDY ) ) ;

  rcc->cfgr &= 0x00000300; /* all prescalers to 0, clock source to HSI */

  rcc->cr &= 0xF0F0FFFD;   /* turn off all clocks and PLL except HSI */

  status = set_system_clk( STM32F4_SYSCLK / 1000000L,
    STM32F4_HSE_OSCILLATOR / 1000000L,
    1 );

  assert( rtems_is_status_successful( status ) );
}

/**
 * @brief Sets up clocks configuration.
 *
 * Set up clocks configuration to achieve desired system clock
 * as close as possible with simple math.
 *
 * Limitations:
 * It is assumed that 1MHz resolution is enough.
 * Best fits for the clocks are achieved with multiplies of 42MHz.
 * Even though APB1, APB2 and AHB are calculated user is still required
 * to provide correct values for the bsp configuration for the:
 * STM32F4_PCLK1
 * STM32F4_PCLK2
 * STM32F4_HCLK
 * as those are used for the peripheral clocking calculations.
 *
 * @param sys_clk Desired system clock in MHz.
 * @param hse_clk External clock speed in MHz.
 * @param hse_flag Flag determining which clock source to use, 1 for HSE,
 *                 0 for HSI.
 *
 * @retval RTEMS_SUCCESSFUL Configuration has been succesfully aplied for the
 *                          requested clock speed.
 * @retval RTEMS_TIMEOUT HSE clock didn't start or PLL didn't lock.
 * @retval RTEMS_INVALID_NUMBER Requested clock speed is out of range.
 */
static rtems_status_code set_system_clk(
  uint32_t sys_clk,
  uint32_t hse_clk,
  uint32_t hse_flag
)
{
  volatile stm32f4_rcc   *rcc = STM32F4_RCC;
  volatile stm32f4_flash *flash = STM32F4_FLASH;
  long                    timeout = 0;

  int src_clk = 0;

  uint32_t pll_m = 0;
  uint32_t pll_n = 0;
  uint32_t pll_p = 0;
  uint32_t pll_q = 0;

  uint32_t ahbpre = 0;
  uint32_t apbpre1 = 0;
  uint32_t apbpre2 = 0;

  if ( sys_clk == 16 && hse_clk != 16 ) {
    /* Revert to reset values */
    rcc->cr |= RCC_CR_HSION;   /* turn on HSI */

    while ( !( rcc->cr & RCC_CR_HSIRDY ) ) ;

    /* all prescalers to 0, clock source to HSI */
    rcc->cfgr &= 0x00000300 | RCC_CFGR_SW_HSI;
    rcc->cr &= 0xF0F0FFFD;   /* turn off all clocks and PLL except HSI */
    flash->acr = 0; /* slow clock so no cache, no prefetch, no latency */

    return RTEMS_SUCCESSFUL;
  }

  if ( sys_clk == hse_clk ) {
    /* Revert to reset values */
    rcc->cr |= RCC_CR_HSEON;   /* turn on HSE */
    timeout = 400;

    while ( !( rcc->cr & RCC_CR_HSERDY ) && --timeout ) ;

    assert( timeout != 0 );

    if ( timeout == 0 ) {
      return RTEMS_TIMEOUT;
    }

    /* all prescalers to 0, clock source to HSE */
    rcc->cfgr &= 0x00000300;
    rcc->cfgr |= RCC_CFGR_SW_HSE;
    /* turn off all clocks and PLL except HSE */
    rcc->cr &= 0xF0F0FFFC | RCC_CR_HSEON;
    flash->acr = 0; /* slow clock so no cache, no prefetch, no latency */

    return RTEMS_SUCCESSFUL;
  }

  /*
   * Lets use 1MHz input for PLL so we get higher VCO output
   * this way we get better value for the PLL_Q divader for the USB
   *
   * Though you might want to use 2MHz as per CPU specification:
   *
   * Caution:The software has to set these bits correctly to ensure
   * that the VCO input frequency ranges from 1 to 2 MHz.
   * It is recommended to select a frequency of 2 MHz to limit PLL jitter.
   */

  if ( sys_clk > 180 ) {
    return RTEMS_INVALID_NUMBER;
  } else if ( sys_clk >= 96 ) {
    pll_n = sys_clk << 1;
    pll_p = RCC_PLLCFGR_PLLP_BY_2;
  } else if ( sys_clk >= 48 ) {
    pll_n = sys_clk << 2;
    pll_p = RCC_PLLCFGR_PLLP_BY_4;
  } else if ( sys_clk >= 24 ) {
    pll_n = sys_clk << 3;
    pll_p = RCC_PLLCFGR_PLLP_BY_8;
  } else {
    return RTEMS_INVALID_NUMBER;
  }

  if ( hse_clk == 0 || hse_flag == 0 ) {
    src_clk = 16;
    hse_flag = 0;
  } else {
    src_clk = hse_clk;
  }

  pll_m = src_clk; /* divide by the oscilator speed in MHz */

  /* pll_q is a prescaler from VCO for the USB OTG FS, SDIO and RNG,
   * best if results in the 48MHz for the USB
   */
  pll_q = ( (long) ( src_clk * pll_n ) ) / pll_m / 48;

  if ( pll_q < 2 ) {
    pll_q = 2;
  }

  /* APB1 prescaler, APB1 clock must be < 42MHz */
  apbpre1 = ( sys_clk * 100 ) / 42;

  if ( apbpre1 <= 100 ) {
    apbpre1 = RCC_CFGR_PPRE1_BY_1;
  } else if ( apbpre1 <= 200 ) {
    apbpre1 = RCC_CFGR_PPRE1_BY_2;
  } else if ( apbpre1 <= 400 ) {
    apbpre1 = RCC_CFGR_PPRE1_BY_4;
  } else if ( apbpre1 <= 800 ) {
    apbpre1 = RCC_CFGR_PPRE1_BY_8;
  } else if ( apbpre1 ) {
    apbpre1 = RCC_CFGR_PPRE1_BY_16;
  }

  /* APB2 prescaler, APB2 clock must be < 84MHz */
  apbpre2 = ( sys_clk * 100 ) / 84;

  if ( apbpre2 <= 100 ) {
    apbpre2 = RCC_CFGR_PPRE2_BY_1;
  } else if ( apbpre2 <= 200 ) {
    apbpre2 = RCC_CFGR_PPRE2_BY_2;
  } else if ( apbpre2 <= 400 ) {
    apbpre2 = RCC_CFGR_PPRE2_BY_4;
  } else if ( apbpre2 <= 800 ) {
    apbpre2 = RCC_CFGR_PPRE2_BY_8;
  } else {
    apbpre2 = RCC_CFGR_PPRE2_BY_16;
  }

  rcc->cr |= RCC_CR_HSION;   /* turn on HSI */

  while ( ( !( rcc->cr & RCC_CR_HSIRDY ) ) ) ;

  /* all prescalers to 0, clock source to HSI */
  rcc->cfgr &= 0x00000300;
  rcc->cfgr |= RCC_CFGR_SW_HSI;

  while ( ( ( rcc->cfgr & RCC_CFGR_SWS_MSK ) != RCC_CFGR_SWS_HSI ) ) ;

  /* turn off PLL */
  rcc->cr &= ~( RCC_CR_PLLON | RCC_CR_PLLRDY );

  /* turn on HSE */
  if ( hse_flag ) {
    rcc->cr |= RCC_CR_HSEON;
    timeout = 400;

    while ( ( !( rcc->cr & RCC_CR_HSERDY ) ) && timeout-- ) ;

    assert( timeout != 0 );

    if ( timeout == 0 ) {
      return RTEMS_TIMEOUT;
    }
  }

  rcc->pllcfgr &= 0xF0BC8000; /* clear PLL prescalers */

  /* set pll parameters */
  rcc->pllcfgr |= RCC_PLLCFGR_PLLM( pll_m ) | /* input divider */
                  RCC_PLLCFGR_PLLN( pll_n ) | /* multiplier */
                  pll_p |                     /* output divider from table */
                                              /* HSE v HSI */
                  ( hse_flag ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI )
                  |
                  RCC_PLLCFGR_PLLQ( pll_q );    /* PLLQ divider */

  /* set prescalers for the internal busses */
  rcc->cfgr |= apbpre1 |
               apbpre2 |
               ahbpre;

  /*
   * Set flash parameters, hard coded for now for fast system clocks.
   * TODO implement some math to use flash on as low latancy as possible
   */
  flash->acr = STM32F4_FLASH_ACR_LATENCY( 5 ) | /* latency */
               STM32F4_FLASH_ACR_ICEN |       /* instruction cache */
               STM32F4_FLASH_ACR_DCEN |        /* data cache */
               STM32F4_FLASH_ACR_PRFTEN;

  /* turn on PLL */
  rcc->cr |= RCC_CR_PLLON;
  timeout = 40000;

  while ( ( !( rcc->cr & RCC_CR_PLLRDY ) ) && --timeout ) ;
  
  assert( timeout != 0 );

  if ( timeout == 0 ) {
    return RTEMS_TIMEOUT;
  }

  /* clock source to PLL */
  rcc->cfgr = ( rcc->cfgr & ~RCC_CFGR_SW_MSK ) | RCC_CFGR_SW_PLL;

  while ( ( ( rcc->cfgr & RCC_CFGR_SWS_MSK ) != RCC_CFGR_SWS_PLL ) ) ;

  return RTEMS_SUCCESSFUL;
}

#endif /* STM32F4_FAMILY_F4XXXX */

#ifdef STM32F4_FAMILY_F10XXX

static void init_main_osc( void )
{

}

#endif /* STM32F4_FAMILY_F10XXX */

void bsp_start( void )
{
  init_main_osc();

  stm32f4_gpio_set_config_array( &stm32f4_start_config_gpio[ 0 ] );

  bsp_interrupt_initialize();
}
