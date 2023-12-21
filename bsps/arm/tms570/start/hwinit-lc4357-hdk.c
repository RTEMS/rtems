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
