/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief First configurations and initializations to the correct
 *              functionality of the board.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/io.h>

/**
 * @brief Initializes the oscillator according to the lpc176x board.
 */
static BSP_START_TEXT_SECTION void lpc176x_init_main_oscillator( void )
{
  if ( ( LPC176X_SCB.scs & LPC176X_SCB_SCS_OSC_STATUS ) == 0u ) {
    LPC176X_SCB.scs |= LPC176X_SCB_SCS_OSC_ENABLE;

    while ( ( LPC176X_SCB.scs & LPC176X_SCB_SCS_OSC_STATUS ) == 0u ) {
      /* Wait. */
    }
  }

  /* else implies that the oscillator is initialized. Also,
     there is nothing to do. */
}

/**
 * @brief Sets the PLL configuration.
 *
 * @param pll Value to set.
 * @param val Set value.
 */
static BSP_START_TEXT_SECTION void lpc176x_pll_config( const uint32_t val )
{
  ( LPC176X_SCB.pll_0 ).con = val;
  /* The two register writes must be in correct sequence. */
  ( LPC176X_SCB.pll_0 ).feed = LPC176X_PLL0CON;
  ( LPC176X_SCB.pll_0 ).feed = LPC176X_PLL0CFG;
}

/**
 * @brief Sets the PLL.
 *
 * @param msel Multiplier value.
 * @param psel Divider value.
 * @param cclkdiv Divisor clock.
 */
static BSP_START_TEXT_SECTION void lpc176x_set_pll(
  const unsigned msel,
  const unsigned psel,
  const unsigned cclkdiv
)
{
  const uint32_t pllcfg = LPC176X_PLL_SEL_MSEL( msel ) |
                          LPC176X_PLL_SEL_PSEL( psel );
  const uint32_t pllstat = LPC176X_PLL_STAT_PLLE | LPC176X_PLL_STAT_PLOCK |
                           pllcfg;
  const uint32_t cclksel_cclkdiv = LPC176X_SCB_CCLKSEL_CCLKDIV( cclkdiv );

  if ( ( LPC176X_SCB.pll_0 ).stat != pllstat
       || LPC176X_SCB.cclksel != cclksel_cclkdiv
       || LPC176X_SCB.clksrcsel != LPC176X_SCB_CLKSRCSEL_CLKSRC ) {
    lpc176x_pll_config( ( LPC176X_SCB.pll_0 ).con & ~LPC176X_PLL_CON_PLLC );

    /* Turn off USB.  */
    LPC176X_SCB.usbclksel = 0u;

    /* Disable PLL. */
    lpc176x_pll_config( 0u );

    /* Use SYSCLK for CCLK. */
    LPC176X_SCB.cclksel = LPC176X_SCB_CCLKSEL_CCLKDIV( 0u );

    /* Set the CCLK, PCLK and EMCCLK divider. */
    LPC176X_SCB.cclksel = cclksel_cclkdiv;

    /* Select main oscillator as clock source. */
    LPC176X_SCB.clksrcsel = LPC176X_SCB_CLKSRCSEL_CLKSRC;

    /* The two register writes must be in correct sequence. */
    /* Set PLL configuration. */
    ( LPC176X_SCB.pll_0 ).cfg = pllcfg;
    ( LPC176X_SCB.pll_0 ).feed = LPC176X_PLL0CON;
    ( LPC176X_SCB.pll_0 ).feed = LPC176X_PLL0CFG;

    /* Enable PLL. */
    lpc176x_pll_config( LPC176X_PLL_CON_PLLE );

    /* Wait for lock. */
    while ( ( ( LPC176X_SCB.pll_0 ).stat & LPC176X_PLL_STAT_PLOCK ) == 0u ) {
      /* Wait */
    }

    /* Connect PLL. */
    lpc176x_pll_config( ( LPC176X_PLL_CON_PLLE | LPC176X_PLL_CON_PLLC ) );

    /* Wait for connected and enabled. */
    while ( ( ( LPC176X_SCB.pll_0 ).stat & ( LPC176X_PLL_STAT_PLLE |
                                             LPC176X_PLL_STAT_PLLC ) ) ==
            0u ) {
      /* Wait  */
    }
  }

  /* else implies that the pll has a wrong value. Also,
     there is nothing to do. */
}

/**
 * @brief Pll initialization.
 */
static BSP_START_TEXT_SECTION void lpc176x_init_pll( void )
{
#if ( LPC176X_OSCILLATOR_MAIN == 12000000u )
#if ( LPC176X_CCLK == 96000000U )
  lpc176x_set_pll( 11u, 0u, 2u );
#else
#error "unexpected CCLK"
#endif
#else
#error "unexpected main oscillator frequency"
#endif
}

/**
 * @brief Memory map initialization.
 */
static BSP_START_TEXT_SECTION void lpc176x_init_memory_map( void )
{
  LPC176X_SCB.memmap = LPC176X_SCB_MEMMAP_MAP;
}

/**
 * @brief Memory accelerator initialization.
 */
static BSP_START_TEXT_SECTION void lpc176x_init_memory_accelerator( void )
{
#if ( LPC176X_CCLK <= 20000000U )
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x0U );
#elif ( LPC176X_CCLK <= 40000000U )
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x1U );
#elif ( LPC176X_CCLK <= 60000000U )
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x2U );
#elif ( LPC176X_CCLK <= 80000000U )
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x3U );
#elif ( LPC176X_CCLK <= 100000000U )
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x4U );
#else
  LPC176X_SCB.flashcfg = LPC176X_SCB_FLASHCFG_FLASHTIM( 0x5U );
#endif
}

/**
 * @brief Stops the gpdma device.
 */
static BSP_START_TEXT_SECTION void lpc176x_stop_gpdma( void )
{
#ifdef LPC176X_STOP_GPDMA

  bool has_power = ( LPC176X_SCB.pconp & LPC176X_SCB_PCONP_GPDMA ) != 0u;

  if ( has_power ) {
    GPDMA_CONFIG = 0u;
    LPC176X_SCB.pconp &= ~LPC176X_SCB_PCONP_GPDMA;
  }

  /* else implies that the current module (gpdma) is turn off. Also,
     there is nothing to do. */

#endif
}

/**
 * @brief Stops the usb device.
 */
static BSP_START_TEXT_SECTION void lpc176x_stop_usb( void )
{
#ifdef LPC176X_STOP_USB

  bool has_power = ( LPC176X_SCB.pconp & LPC176X_SCB_PCONP_USB ) != 0u;

  if ( has_power ) {
    OTG_CLK_CTRL = 0u;

    LPC176X_SCB.pconp &= ~LPC176X_SCB_PCONP_USB;
    LPC176X_SCB.usbclksel = 0u;
  }

  /* else implies that the current module (usb) is turn off. Also,
     there is nothing to do. */
#endif
}

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
  lpc176x_init_main_oscillator();
  lpc176x_init_pll();
}

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
  lpc176x_init_memory_map();
  lpc176x_init_memory_accelerator();
  lpc176x_stop_gpdma();
  lpc176x_stop_usb();
  bsp_start_copy_sections();
  bsp_start_clear_bss();

  /* At this point we can use objects outside the .start section  */
}