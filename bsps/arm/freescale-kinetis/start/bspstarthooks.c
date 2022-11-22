/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief First configurations and initializations to the correct
 *              functionality of the board.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on TMS570 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#include <bsp.h>
#include <bsp/start.h>
#include <bsp/kinetis.h>
#include <bsp/psram.h>

#define DISABLE_WDOG    1

#define CLOCK_SETUP     1
/* Predefined clock setups
   0 ... Multipurpose Clock Generator (MCG) in FLL Engaged Internal (FEI) mode
         Reference clock source for MCG module is the slow internal clock source 32.768kHz
         Core clock = 41.94MHz, BusClock = 41.94MHz
   1 ... Multipurpose Clock Generator (MCG) in PLL Engaged External (PEE) mode
         Reference clock source for MCG module is an external crystal 8MHz
         Core clock = 72MHz, BusClock = 72MHz
   2 ... Multipurpose Clock Generator (MCG) in Bypassed Low Power External (BLPE) mode
         Core clock/Bus clock derived directly from an external crystal 8MHz with no multiplication
         Core clock = 8MHz, BusClock = 8MHz
*/

/*----------------------------------------------------------------------------
  Define clock source values
 *----------------------------------------------------------------------------*/
#if (CLOCK_SETUP == 0)
    #define CPU_XTAL_CLK_HZ                 8000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            41943040u /* Default System clock value */
#elif (CLOCK_SETUP == 1)
    #define CPU_XTAL_CLK_HZ                 8000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            72000000u /* Default System clock value */
#elif (CLOCK_SETUP == 2)
    #define CPU_XTAL_CLK_HZ                 8000000u /* Value of the external crystal or oscillator clock frequency in Hz */
    #define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
    #define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
    #define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
    #define DEFAULT_SYSTEM_CLOCK            8000000u /* Default System clock value */
#endif /* (CLOCK_SETUP == 2) */

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t system_core_clock = DEFAULT_SYSTEM_CLOCK;

#pragma GCC push_options
#pragma GCC optimize("O0")
/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

static void system_clock_initialize(void) {
#if (DISABLE_WDOG)
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  KINETIS_WDOG->unlock = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  KINETIS_WDOG->unlock  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  KINETIS_WDOG->stat_ctrl_high = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
#if (CLOCK_SETUP == 0)
  /* KINETIS_SIM->clk_div1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  KINETIS_SIM->clk_div1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FEI Mode */
  /* KINETIS_MCG->ctrl1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
  KINETIS_MCG->ctrl1 = (uint8_t)0x06u;
  /* KINETIS_MCG->ctrl2: ??=0,??=0,RANGE0=0,HGO=0,EREFS=0,LP=0,IRCS=0 */
  KINETIS_MCG->ctrl2 = (uint8_t)0x00u;
  /* MCG_C4: DMX32=0,DRST_DRS=1 */
  KINETIS_MCG->ctrl4 = (uint8_t)((KINETIS_MCG->ctrl4 & (uint8_t)~(uint8_t)0xC0u) | (uint8_t)0x20u);
  /* KINETIS_MCG->ctrl5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=0 */
  KINETIS_MCG->ctrl5 = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  KINETIS_MCG->ctrl6 = (uint8_t)0x00u;
  while((KINETIS_MCG->status & MCG_S_IREFST_MASK) == 0u) { /* Check that the source of the FLL reference clock is the internal reference clock. */
  }
  while((KINETIS_MCG->status & 0x0Cu) != 0x00u) {    /* Wait until output of the FLL is selected */
  }
#elif (CLOCK_SETUP == 1)
  /* KINETIS_SIM->clk_div1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  KINETIS_SIM->clk_div1 = (uint32_t)0x00220000u; /* Update system prescalers */
  /* Switch to FBE Mode */
  /* OSC.CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC->cr = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl7: OSCSEL=0 */
  KINETIS_MCG->ctrl7 = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  KINETIS_MCG->ctrl2 = (uint8_t)0x24u;
  /* KINETIS_MCG->ctrl1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  KINETIS_MCG->ctrl1 = (uint8_t)0x9Au;
  /* KINETIS_MCG->ctrl4: DMX32=0,DRST_DRS=0 */
  KINETIS_MCG->ctrl4 &= (uint8_t)~(uint8_t)0xE0u;
  /* KINETIS_MCG->ctrl5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=3 */
  KINETIS_MCG->ctrl5 = (uint8_t)0x03u;
  /* KINETIS_MCG->ctrl6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  KINETIS_MCG->ctrl6 = (uint8_t)0x00u;
  while((KINETIS_MCG->status & MCG_S_OSCINIT0_MASK) == 0u) { /* Check that the oscillator is running */
  }
  while((KINETIS_MCG->status & MCG_S_IREFST_MASK) != 0u) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((KINETIS_MCG->status & 0x0Cu) != 0x08u) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to PBE Mode */
  /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=1 */
  KINETIS_MCG->ctrl5 = (uint8_t)0x03u;
  /* KINETIS_MCG->ctrl6: LOLIE=0,PLLS=1,CME=0,VDIV0=1 */
  KINETIS_MCG->ctrl6 = (uint8_t)0x4Cu;
  while((KINETIS_MCG->status & MCG_S_PLLST_MASK) == 0u) { /* Wait until the source of the PLLS clock has switched to the PLL */
  }
  while((KINETIS_MCG->status & MCG_S_LOCK0_MASK) == 0u) { /* Wait until locked */
  }
  /* Switch to PEE Mode */
  /* KINETIS_MCG->ctrl1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  KINETIS_MCG->ctrl1 = (uint8_t)0x1Au;
  while((KINETIS_MCG->status & 0x0Cu) != 0x0Cu) {    /* Wait until output of the PLL is selected */
  }
  while((KINETIS_MCG->status & MCG_S_LOCK0_MASK) == 0u) { /* Wait until locked */
  }
#elif (CLOCK_SETUP == 2)
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=1,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  KINETIS_SIM->clk_div1 = (uint32_t)0x00110000u; /* Update system prescalers */
  /* Switch to FBE Mode */
  /* OSC.CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC->cr = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl7: OSCSEL=0 */
  KINETIS_MCG->ctrl7 = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  KINETIS_MCG->ctrl2 = (uint8_t)0x24u;
  /* KINETIS_MCG->ctrl1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  KINETIS_MCG->ctrl1 = (uint8_t)0x9Au;
  /* KINETIS_MCG->ctrl4: DMX32=0,DRST_DRS=0 */
  KINETIS_MCG->ctrl4 &= (uint8_t)~(uint8_t)0xE0u;
  /* KINETIS_MCG->ctrl5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV0=0 */
  KINETIS_MCG->ctrl5 = (uint8_t)0x00u;
  /* KINETIS_MCG->ctrl6: LOLIE=0,PLLS=0,CME=0,VDIV0=0 */
  KINETIS_MCG->ctrl6 = (uint8_t)0x00u;
  while((KINETIS_MCG->status & MCG_S_OSCINIT0_MASK) == 0u) { /* Check that the oscillator is running */
  }
  while((KINETIS_MCG->status & MCG_S_IREFST_MASK) != 0u) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((KINETIS_MCG->status & 0x0CU) != 0x08u) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to BLPE Mode */
  /* KINETIS_MCG->ctrl2: ??=0,??=0,RANGE0=2,HGO=0,EREFS=1,LP=0,IRCS=0 */
  KINETIS_MCG->ctrl2 = (uint8_t)0x24u;
#endif /* (CLOCK_SETUP == 2) */
}
#pragma GCC pop_options

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
    system_clock_initialize();
    flexbus_gpio_init();
    flexbus_clock_enable();
    flexbus_init(5);
}

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
    bsp_start_copy_sections();
    bsp_start_clear_bss();
    /* At this point we can use objects outside the .start section  */
}

