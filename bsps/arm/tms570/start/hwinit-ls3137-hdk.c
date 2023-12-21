/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains parts of the system initialization.
 */

/*
 * Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <bsp/tms570.h>
#include <bsp/tms570_hwinit.h>

enum tms570_system_clock_source {
  TMS570_SYS_CLK_SRC_OSC = 0U,          /**< Alias for oscillator clock Source                */
  TMS570_SYS_CLK_SRC_PLL1 = 1U,         /**< Alias for Pll1 clock Source                      */
  TMS570_SYS_CLK_SRC_EXTERNAL1 = 3U,    /**< Alias for external clock Source                  */
  TMS570_SYS_CLK_SRC_LPO_LOW = 4U,      /**< Alias for low power oscillator low clock Source  */
  TMS570_SYS_CLK_SRC_LPO_HIGH = 5U,     /**< Alias for low power oscillator high clock Source */
  TMS570_SYS_CLK_SRC_PLL2 = 6U,         /**< Alias for Pll2 clock Source                      */
  TMS570_SYS_CLK_SRC_EXTERNAL2 = 7U,    /**< Alias for external 2 clock Source                */
  TMS570_SYS_CLK_SRC_VCLK = 9U          /**< Alias for synchronous VCLK1 clock Source         */
};

/**
 * @brief Setup all system PLLs (HCG:setupPLL)
 *
 */
void tms570_pll_init( void )
{
  uint32_t pll12_dis = 0x42;

  /* Disable PLL1 and PLL2 */
  TMS570_SYS1.CSDISSET = pll12_dis;

  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( ( TMS570_SYS1.CSDIS & pll12_dis ) != pll12_dis ) {
    /* Wait */
  }

  /* Clear Global Status Register */
  TMS570_SYS1.GLBSTAT = TMS570_SYS1_GLBSTAT_FBSLIP |
                        TMS570_SYS1_GLBSTAT_RFSLIP |
                        TMS570_SYS1_GLBSTAT_OSCFAIL;
  /** - Configure PLL control registers */
  /** @b Initialize @b Pll1: */

  /* Setup pll control register 1 */
  TMS570_SYS1.PLLCTL1 = TMS570_SYS1_PLLCTL1_ROS * 0 |
                        TMS570_SYS1_PLLCTL1_MASK_SLIP( 1 ) |
                        TMS570_SYS1_PLLCTL1_PLLDIV( 0x1f ) | /* max value */
                        TMS570_SYS1_PLLCTL1_ROF * 0 |
                        TMS570_SYS1_PLLCTL1_REFCLKDIV( 6 - 1 ) |
                        TMS570_SYS1_PLLCTL1_PLLMUL( ( 120 - 1 ) << 8 );

  /* Setup pll control register 2 */
  TMS570_SYS1.PLLCTL2 = TMS570_SYS1_PLLCTL2_FMENA * 0 |
                        TMS570_SYS1_PLLCTL2_SPREADINGRATE( 255 ) |
                        TMS570_SYS1_PLLCTL2_MULMOD( 7 ) |
                        TMS570_SYS1_PLLCTL2_ODPLL( 2 - 1 ) |
                        TMS570_SYS1_PLLCTL2_SPR_AMOUNT( 61 );

  /** @b Initialize @b Pll2: */

  /* Setup pll2 control register */
  TMS570_SYS2.PLLCTL3 = TMS570_SYS2_PLLCTL3_ODPLL2( 2 - 1 ) |
                        TMS570_SYS2_PLLCTL3_PLLDIV2( 0x1F ) | /* max value */
                        TMS570_SYS2_PLLCTL3_REFCLKDIV2( 6 - 1 ) |
                        TMS570_SYS2_PLLCTL3_PLLMUL2( ( 120 - 1 ) << 8 );

  /** - Enable PLL(s) to start up or Lock */
  TMS570_SYS1.CSDIS = 0x00000000 | /* CLKSR0 on */
                      0x00000000 | /* CLKSR1 on */
                      0x00000008 | /* CLKSR3 off */
                      0x00000000 | /* CLKSR4 on */
                      0x00000000 | /* CLKSR5 on */
                      0x00000000 | /* CLKSR6 on */
                      0x00000080;  /* CLKSR7 off */
}

/**
 * @brief Setup chip clocks including to wait for PLLs locks (HCG:mapClocks)
 *
 */
/* SourceId : SYSTEM_SourceId_005 */
/* DesignId : SYSTEM_DesignId_005 */
/* Requirements : HL_SR469 */
void tms570_map_clock_init( void )
{
  uint32_t sys_csvstat, sys_csdis;

  /** @b Initialize @b Clock @b Tree: */
  /** - Disable / Enable clock domain */
  TMS570_SYS1.CDDIS = ( 0U << 4U ) |  /* AVCLK 1 OFF */
                      ( 0U << 5U ) |  /* AVCLK 2 OFF */
                      ( 0U << 8U ) |  /* VCLK3 OFF */
                      ( 0U << 9U ) |  /* VCLK4 OFF */
                      ( 1U << 10U ) | /* AVCLK 3 OFF */
                      ( 0U << 11U );  /* AVCLK 4 OFF */

  /* Work Around for Errata SYS#46:
   *
   * Errata Description:
   *            Clock Source Switching Not Qualified with Clock Source Enable And Clock Source Valid
   * Workaround:
   *            Always check the CSDIS register to make sure the clock source is turned on and check
   * the CSVSTAT register to make sure the clock source is valid. Then write to GHVSRC to switch the clock.
   */
  /** - Wait for until clocks are locked */
  sys_csvstat = TMS570_SYS1.CSVSTAT;
  sys_csdis = TMS570_SYS1.CSDIS;

  while ( ( sys_csvstat & ( ( sys_csdis ^ 0xFFU ) & 0xFFU ) ) !=
          ( ( sys_csdis ^ 0xFFU ) & 0xFFU ) ) {
    sys_csvstat = TMS570_SYS1.CSVSTAT;
    sys_csdis = TMS570_SYS1.CSDIS;
  } /* Wait */

  /* Now the PLLs are locked and the PLL outputs can be sped up */
  /* The R-divider was programmed to be 0xF. Now this divider is changed to programmed value */
  TMS570_SYS1.PLLCTL1 =
    ( TMS570_SYS1.PLLCTL1 & ~TMS570_SYS1_PLLCTL1_PLLDIV( 0x1F ) ) |
    TMS570_SYS1_PLLCTL1_PLLDIV( 1 - 1 );
  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
  TMS570_SYS2.PLLCTL3 =
    ( TMS570_SYS2.PLLCTL3 & ~TMS570_SYS2_PLLCTL3_PLLDIV2( 0x1F ) ) |
    TMS570_SYS2_PLLCTL3_PLLDIV2( 1 - 1 );

  /* Enable/Disable Frequency modulation */
  TMS570_SYS1.PLLCTL2 &= ~TMS570_SYS1_PLLCTL2_FMENA;

  /** - Map device clock domains to desired sources and configure top-level dividers */
  /** - All clock domains are working off the default clock sources until now */
  /** - The below assignments can be easily modified using the HALCoGen GUI */

  /** - Setup GCLK, HCLK and VCLK clock source for normal operation, power down mode and after wakeup */
  TMS570_SYS1.GHVSRC = TMS570_SYS1_GHVSRC_GHVWAKE( TMS570_SYS_CLK_SRC_OSC ) |
                       TMS570_SYS1_GHVSRC_HVLPM( TMS570_SYS_CLK_SRC_OSC ) |
                       TMS570_SYS1_GHVSRC_GHVSRC( TMS570_SYS_CLK_SRC_PLL1 );

  /** - Setup synchronous peripheral clock dividers for VCLK1, VCLK2, VCLK3 */
  TMS570_SYS1.CLKCNTL =
    ( TMS570_SYS1.CLKCNTL & ~TMS570_SYS1_CLKCNTL_VCLK2R( 0xF ) ) |
    TMS570_SYS1_CLKCNTL_VCLK2R( 1 );

  TMS570_SYS1.CLKCNTL =
    ( TMS570_SYS1.CLKCNTL & ~TMS570_SYS1_CLKCNTL_VCLKR( 0xF ) ) |
    TMS570_SYS1_CLKCNTL_VCLKR( 1 );

  TMS570_SYS2.CLK2CNTRL =
    ( TMS570_SYS2.CLK2CNTRL & ~TMS570_SYS2_CLK2CNTRL_VCLK3R( 0xF ) ) |
    TMS570_SYS2_CLK2CNTRL_VCLK3R( 1 );

  TMS570_SYS2.CLK2CNTRL = ( TMS570_SYS2.CLK2CNTRL & 0xFFFFF0FFU ) |
                          ( 1U << 8U ); /* FIXME: unknown in manual*/

  /** - Setup RTICLK1 and RTICLK2 clocks */
  TMS570_SYS1.RCLKSRC = ( 1U << 24U ) |
                        ( TMS570_SYS_CLK_SRC_VCLK << 16U ) | /* FIXME: not in manual */
                        TMS570_SYS1_RCLKSRC_RTI1DIV( 1 ) |
                        TMS570_SYS1_RCLKSRC_RTI1SRC( TMS570_SYS_CLK_SRC_VCLK );

  /** - Setup asynchronous peripheral clock sources for AVCLK1 and AVCLK2 */
  TMS570_SYS1.VCLKASRC =
    TMS570_SYS1_VCLKASRC_VCLKA2S( TMS570_SYS_CLK_SRC_VCLK ) |
    TMS570_SYS1_VCLKASRC_VCLKA1S( TMS570_SYS_CLK_SRC_VCLK );

  TMS570_SYS2.VCLKACON1 = TMS570_SYS2_VCLKACON1_VCLKA4R( 1 - 1 ) |
                          TMS570_SYS2_VCLKACON1_VCLKA4_DIV_CDDIS * 0 |
                          TMS570_SYS2_VCLKACON1_VCLKA4S(
    TMS570_SYS_CLK_SRC_VCLK ) |
                          TMS570_SYS2_VCLKACON1_VCLKA3R( 1 - 1 ) |
                          TMS570_SYS2_VCLKACON1_VCLKA3_DIV_CDDIS * 0 |
                          TMS570_SYS2_VCLKACON1_VCLKA3S(
    TMS570_SYS_CLK_SRC_VCLK );
}
