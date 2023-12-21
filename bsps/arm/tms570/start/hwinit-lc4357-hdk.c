/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains parts of the system initialization.
 */

/*
 * Copyright (C) 2022 Airbus U.S. Space & Defense, Inc
 * Copyright (C) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
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
#include <bsp/tms570-pinmux.h>

typedef enum Tms570ClockDisableSources {
    TMS570_CLKDIS_SRC_OSC  = 0x01, ///< External high-speed oscillator as clock source
    TMS570_CLKDIS_SRC_PLL1 = 0x02, 
    TMS570_CLKDIS_SRC_RESERVED = 0x04, ///< reserved. not tied to actual clock source
    TMS570_CLKDIS_SRC_EXT_CLK1 = 0x08, 
    TMS570_CLKDIS_SRC_LOW_FREQ_LPO = 0x10, 
    TMS570_CLKDIS_SRC_HIGH_FREQ_LPO = 0x20, 
    TMS570_CLKDIS_SRC_PLL2 = 0x40, 
    TMS570_CLKDIS_SRC_EXT_CLK2 = 0x80, 
} Tms570ClockDisableSources;

// Source selection for G, H, and V clocks SYS1.GHVSRC reg
typedef enum Tms570GhvClockSources {
  TMS570_SYS_CLK_SRC_OSC = 0U,          /**< Alias for oscillator clock Source                */
  TMS570_SYS_CLK_SRC_PLL1 = 1U,         /**< Alias for Pll1 clock Source                      */
  TMS570_SYS_CLK_SRC_EXTERNAL1 = 3U,    /**< Alias for external clock Source                  */
  TMS570_SYS_CLK_SRC_LPO_LOW = 4U,      /**< Alias for low power oscillator low clock Source  */
  TMS570_SYS_CLK_SRC_LPO_HIGH = 5U,     /**< Alias for low power oscillator high clock Source */
  TMS570_SYS_CLK_SRC_PLL2 = 6U,         /**< Alias for Pll2 clock Source                      */
  TMS570_SYS_CLK_SRC_EXTERNAL2 = 7U,    /**< Alias for external 2 clock Source                */
  TMS570_SYS_CLK_SRC_VCLK = 9U          /**< Alias for synchronous VCLK1 clock Source         */
} Tms570GhvClockSources;

/*
 * The next construct allows to compute values for individual
 * PINMMR registers based on the multiple processing
 * complete pin functions list at compile time.
 * Each line computes 32-bit value which selects function
 * of consecutive four pins. Each pin function is defined
 * by single byte.
 */
static const uint32_t tms570_pinmmr_init_data[] = {
  TMS570_PINMMR_REG_VAL( 0, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 1, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 2, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 3, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 4, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 5, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 6, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 7, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 8, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 9, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 10, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 11, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 12, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 13, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 14, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 15, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 16, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 17, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 18, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 19, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 20, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 21, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 22, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 23, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 24, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 25, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 26, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 27, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 28, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 29, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 30, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 31, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 32, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 33, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 34, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 35, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 36, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 37, TMS570LC4357_PINMMR_DEFAULT_INIT_LIST ),
};

void tms570_pinmux_init( void )
{
    tms570_bsp_pinmmr_config(tms570_pinmmr_init_data, 0, RTEMS_ARRAY_SIZE(tms570_pinmmr_init_data));

    tms570_pin_config_prepare();
    TMS570_PINMUX[174] = (TMS570_PINMUX[174] & ~(UINT32_C(0x3) << 8)) | (UINT32_C(0x2) << 8); // emif output-enable bit8= 0, bit9= 1
    tms570_pin_config_complete();
}

void tms570_emif_sdram_init( void )
{
    uint32_t dummy;

    /* Do not run attempt to initialize SDRAM when code is running from it */
    if ( tms570_running_from_sdram() )
        return;

    // Following the initialization procedure as described in EMIF-errata #5 for the tms570lc43
    // at EMIF clock rates >= 40Mhz
    // Note step one of this procedure is running this EMIF initialization sequence before PLL
    // and clocks are mapped/enabled
    // For additional details on startup procedure see tms570lc43 TRM s21.2.5.5.B

    // Set SDRAM timings. These are dependent on the EMIF CLK rate, which = VCLK3
    // Set these based on the final EMIF clock rate once PLL & VCLK is enabled
    TMS570_EMIF.SDTIMR  = (uint32_t)1U << 27U|
                (uint32_t)0U << 24U|
                (uint32_t)0U << 20U|
                (uint32_t)0U << 19U|
                (uint32_t)1U << 16U|
                (uint32_t)1U << 12U|
                (uint32_t)1U << 8U|
                (uint32_t)0U << 4U;

    /* Minimum number of ECLKOUT cycles from Self-Refresh exit to any command */
    // Also set this based on the final EMIF clk
    TMS570_EMIF.SDSRETR = 2;
    // Program the RR Field of SDRCR to provide 200us of initialization time
    // Per Errata#5, for EMIF startup, set this based on the non-VLCK3 clk rate.
    // The Errata is this register must be calculated as `SDRCR = 200us * EMIF_CLK`
    //  (typically this would be `SDRCR = (200us * EMIF_CLK) / 8` ) 
    //  Since the PLL's arent enabled yet, EMIF_CLK would be EXT_OSCIN / 2
    TMS570_EMIF.SDRCR = 1600;

    TMS570_EMIF.SDCR   = ((uint32_t)0U << 31U)|
            ((uint32_t)1U << 14U)|
            ((uint32_t)2U << 9U)|
            ((uint32_t)1U << 8U)|
            ((uint32_t)2U << 4U)|
            ((uint32_t)0); // pagesize = 256

    // Read of SDRAM memory location causes processor to wait until SDRAM Initialization completes
    dummy = *(volatile uint32_t*)TMS570_MEMORY_SDRAM_ORIGIN;
    (void) dummy;

    // Program the RR field to the default Refresh Interval of the SDRAM
    // Program this to the correct interval for the VCLK3/EMIF_CLK rate
    // Do this in the typical way per TRM: SDRCR = ((200us * EMIF_CLK) / 8) + 1
    TMS570_EMIF.SDRCR = 1251;

    /* Place the EMIF in Self Refresh Mode For Clock Change          */
    /* Must only write to the upper byte of the SDCR to avoid        */
    /* a second initialization sequence                              */
    /* The byte address depends on endian (0x3U in LE, 0x00 in BE32) */
    *((volatile unsigned char *)(&TMS570_EMIF.SDCR) + 0x0U) = 0x80;
}

/**
 * @brief Setup all system PLLs (HCG:setupPLL)
 *
 */
void tms570_pll_init( void )
{
    //based on HalCoGen setupPLL method
    uint32_t pll12_dis = TMS570_CLKDIS_SRC_PLL1 | TMS570_CLKDIS_SRC_PLL2;

    /* Disable PLL1 and PLL2 */
    TMS570_SYS1.CSDISSET = pll12_dis;

    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while ( ( TMS570_SYS1.CSDIS & pll12_dis ) != pll12_dis ) {
        /* Wait */
    }

    /* Clear Global Status Register */
    TMS570_SYS1.GLBSTAT = 0x301U;

    // Configure PLL control registers

    /**   - Setup pll control register 1:
     *     - Disable reset on oscillator slip (ROS)
     *     - Enable bypass on pll slip
     *        TODO: desired: switches to OSC when PLL slip detected
     *     - setup Pll output clock divider to max before Lock
     *     - Disable reset on oscillator fail
     *     - Setup reference clock divider
     *     - Setup Pll multiplier
     *
     *     - PLL1: 16MHz OSC in -> 300MHz PLL1 out
     */
    TMS570_SYS1.PLLCTL1 =  (TMS570_SYS1_PLLCTL1_ROS * 0)
                        |  (uint32_t)0x40000000U
                        |  TMS570_SYS1_PLLCTL1_PLLDIV(0x1F)
                        |  (TMS570_SYS1_PLLCTL1_ROF * 0)
                        |  TMS570_SYS1_PLLCTL1_REFCLKDIV(4U - 1U)
                        |  TMS570_SYS1_PLLCTL1_PLLMUL((75U - 1U) << 8);

    /**   - Setup pll control register 2
     *     - Setup spreading rate
     *     - Setup bandwidth adjustment
     *     - Setup internal Pll output divider
     *     - Setup spreading amount
     */
    TMS570_SYS1.PLLCTL2 =  ((uint32_t)255U << 22U)
                        |  ((uint32_t)7U << 12U)
                        |  ((uint32_t)(1U - 1U) << 9U)
                        |  61U;

    // Initialize Pll2

    /**   - Setup pll2 control register :
     *     - setup Pll output clock divider to max before Lock
     *     - Setup reference clock divider
     *     - Setup internal Pll output divider
     *     - Setup Pll multiplier
     */
    TMS570_SYS2.PLLCTL3 = TMS570_SYS2_PLLCTL3_ODPLL2(1U - 1U)
                        | TMS570_SYS2_PLLCTL3_PLLDIV2(0x1FU)
                        | TMS570_SYS2_PLLCTL3_REFCLKDIV2(8U - 1U)
                        | TMS570_SYS2_PLLCTL3_PLLMUL2(( 150U - 1U) << 8 );

    // Enable PLL(s) to start up or Lock
    // Enable all clock sources except the following
    TMS570_SYS1.CSDIS = (TMS570_CLKDIS_SRC_EXT_CLK2 | TMS570_CLKDIS_SRC_EXT_CLK1 | TMS570_CLKDIS_SRC_RESERVED);
}

void tms570_map_clock_init(void)
{
    // based on HalCoGen mapClocks method
    uint32_t sys_csvstat, sys_csdis;

    TMS570_SYS2.HCLKCNTL = 1U;

    /** @b Initialize @b Clock @b Tree: */
    /** - Disable / Enable clock domain */
    TMS570_SYS1.CDDIS = ( 0U << 4U ) |  /* AVCLK 1 ON */
                        ( 1U << 5U ) |  /* AVCLK 2 OFF */
                        ( 0U << 8U ) |  /* VCLK3 ON */
                        ( 0U << 9U ) |  /* VCLK4 ON */
                        ( 0U << 10U ) | /* AVCLK 3 ON */
                        ( 0U << 11U );  /* AVCLK 4 ON */

    /* Work Around for Errata SYS#46:
    * Despite this being a LS3137 errata, hardware testing on the LC4357 indicates this wait is still necessary
    */
    sys_csvstat = TMS570_SYS1.CSVSTAT;
    sys_csdis = TMS570_SYS1.CSDIS;

    while ( ( sys_csvstat & ( ( sys_csdis ^ 0xFFU ) & 0xFFU ) ) !=
            ( ( sys_csdis ^ 0xFFU ) & 0xFFU ) ) {
        sys_csvstat = TMS570_SYS1.CSVSTAT;
        sys_csdis = TMS570_SYS1.CSDIS;
    }

    TMS570_SYS1.GHVSRC =  TMS570_SYS1_GHVSRC_GHVWAKE(TMS570_SYS_CLK_SRC_PLL1)
                        | TMS570_SYS1_GHVSRC_HVLPM(TMS570_SYS_CLK_SRC_PLL1)
                        | TMS570_SYS1_GHVSRC_GHVSRC(TMS570_SYS_CLK_SRC_PLL1);

    /** - Setup RTICLK1 and RTICLK2 clocks */
    TMS570_SYS1.RCLKSRC = ((uint32_t)1U << 24U)        /* RTI2 divider (Not applicable for lock-step device)  */
                        | ((uint32_t)TMS570_SYS_CLK_SRC_VCLK << 16U) /* RTI2 clock source (Not applicable for lock-step device) Field not in TRM? */
                        | ((uint32_t)1U << 8U)         /* RTI1 divider */
                        | ((uint32_t)TMS570_SYS_CLK_SRC_VCLK << 0U); /* RTI1 clock source */

    /** - Setup asynchronous peripheral clock sources for AVCLK1 and AVCLK2 */
    TMS570_SYS1.VCLKASRC =  TMS570_SYS1_VCLKASRC_VCLKA2S(TMS570_SYS_CLK_SRC_VCLK)
                        | TMS570_SYS1_VCLKASRC_VCLKA1S(TMS570_SYS_CLK_SRC_VCLK);

    /** - Setup synchronous peripheral clock dividers for VCLK1, VCLK2, VCLK3 */

    // VCLK2 = PLL1 / HCLK_DIV / 2 = 75MHz
    TMS570_SYS1.CLKCNTL  = (TMS570_SYS1.CLKCNTL & ~TMS570_SYS1_CLKCNTL_VCLK2R(0xF))
                        | TMS570_SYS1_CLKCNTL_VCLK2R(0x1);
    // VLCK1 = PLL1 / HCLK_DIV / 2 = 75MHz
    TMS570_SYS1.CLKCNTL  = (TMS570_SYS1.CLKCNTL & ~TMS570_SYS1_CLKCNTL_VCLKR(0xF))
                        | TMS570_SYS1_CLKCNTL_VCLKR(0x1);

    // VCLK3 = PLL1 / HCLK_DIV / 3 = 50MHz
    TMS570_SYS2.CLK2CNTRL = (TMS570_SYS2.CLK2CNTRL & ~TMS570_SYS2_CLK2CNTRL_VCLK3R(0xF))
                        | TMS570_SYS2_CLK2CNTRL_VCLK3R(0x2);

    TMS570_SYS2.VCLKACON1 =   TMS570_SYS2_VCLKACON1_VCLKA4R(1U - 1U)
                            | (TMS570_SYS2_VCLKACON1_VCLKA4_DIV_CDDIS * 0)
                            | TMS570_SYS2_VCLKACON1_VCLKA4S(TMS570_SYS_CLK_SRC_VCLK)
                            | TMS570_SYS2_VCLKACON1_VCLKA3R(1U - 1U)
                            | (TMS570_SYS2_VCLKACON1_VCLKA3_DIV_CDDIS * 0)
                            | TMS570_SYS2_VCLKACON1_VCLKA3S(TMS570_SYS_CLK_SRC_VCLK);

    /* Now the PLLs are locked and the PLL outputs can be sped up */
    /* The R-divider was programmed to be 0xF. Now this divider is changed to programmed value */
    TMS570_SYS1.PLLCTL1 = (TMS570_SYS1.PLLCTL1 & 0xE0FFFFFFU) | (uint32_t)((uint32_t)(1U - 1U) << 24U);
    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> " Clear and write to the volatile register " */
    TMS570_SYS2.PLLCTL3 = (TMS570_SYS2.PLLCTL3 & 0xE0FFFFFFU) | (uint32_t)((uint32_t)(1U - 1U) << 24U);

    /* Enable/Disable Frequency modulation */
    TMS570_SYS1.PLLCTL2 |= 0x00000000U;
}
