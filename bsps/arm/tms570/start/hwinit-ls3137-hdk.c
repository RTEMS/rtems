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
