/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains parts of the system initialization.
 */

/*
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

/*
 * Definition of fuctions for all pins of TMS570LS3137.
 * This setup correctponds to TMS570LS31x HDK Kit
 */

#define TMS570_PINMMR_INIT_LIST( per_pin_action, common_arg ) \
  per_pin_action( common_arg, TMS570_BALL_W10_GIOB_3 ) \
  per_pin_action( common_arg, TMS570_BALL_A5_GIOA_0 ) \
  per_pin_action( common_arg, TMS570_BALL_C3_MIBSPI3NCS_3 ) \
  per_pin_action( common_arg, TMS570_BALL_B2_MIBSPI3NCS_2 ) \
  per_pin_action( common_arg, TMS570_BALL_C2_GIOA_1 ) \
  per_pin_action( common_arg, TMS570_BALL_E3_HET1_11 ) \
  per_pin_action( common_arg, TMS570_BALL_E5_EMIF_DATA_4 ) \
  per_pin_action( common_arg, TMS570_BALL_F5_EMIF_DATA_5 ) \
  per_pin_action( common_arg, TMS570_BALL_C1_GIOA_2 ) \
  per_pin_action( common_arg, TMS570_BALL_G5_EMIF_DATA_6 ) \
  per_pin_action( common_arg, TMS570_BALL_E1_GIOA_3 ) \
  per_pin_action( common_arg, TMS570_BALL_B5_GIOA_5 ) \
  per_pin_action( common_arg, TMS570_BALL_K5_EMIF_DATA_7 ) \
  per_pin_action( common_arg, TMS570_BALL_B3_HET1_22 ) \
  per_pin_action( common_arg, TMS570_BALL_H3_GIOA_6 ) \
  per_pin_action( common_arg, TMS570_BALL_L5_EMIF_DATA_8 ) \
  per_pin_action( common_arg, TMS570_BALL_M1_GIOA_7 ) \
  per_pin_action( common_arg, TMS570_BALL_M5_EMIF_DATA_9 ) \
  per_pin_action( common_arg, TMS570_BALL_V2_HET1_01 ) \
  per_pin_action( common_arg, TMS570_BALL_U1_HET1_03 ) \
  per_pin_action( common_arg, TMS570_BALL_K18_HET1_00 ) \
  per_pin_action( common_arg, TMS570_BALL_W5_HET1_02 ) \
  per_pin_action( common_arg, TMS570_BALL_V6_HET1_05 ) \
  per_pin_action( common_arg, TMS570_BALL_N5_EMIF_DATA_10 ) \
  per_pin_action( common_arg, TMS570_BALL_T1_HET1_07 ) \
  per_pin_action( common_arg, TMS570_BALL_P5_EMIF_DATA_11 ) \
  per_pin_action( common_arg, TMS570_BALL_V7_HET1_09 ) \
  per_pin_action( common_arg, TMS570_BALL_R5_EMIF_DATA_12 ) \
  per_pin_action( common_arg, TMS570_BALL_R6_EMIF_DATA_13 ) \
  per_pin_action( common_arg, TMS570_BALL_V5_MIBSPI3NCS_1 ) \
  per_pin_action( common_arg, TMS570_BALL_W3_SCIRX ) \
  per_pin_action( common_arg, TMS570_BALL_R7_EMIF_DATA_14 ) \
  per_pin_action( common_arg, TMS570_BALL_N2_SCITX ) \
  per_pin_action( common_arg, TMS570_BALL_G3_MIBSPI1NCS_2 ) \
  per_pin_action( common_arg, TMS570_BALL_N1_HET1_15 ) \
  per_pin_action( common_arg, TMS570_BALL_R8_EMIF_DATA_15 ) \
  per_pin_action( common_arg, TMS570_BALL_R9_ETMTRACECLKIN ) \
  per_pin_action( common_arg, TMS570_BALL_W9_MIBSPI3NENA ) \
  per_pin_action( common_arg, TMS570_BALL_V10_MIBSPI3NCS_0 ) \
  per_pin_action( common_arg, TMS570_BALL_J3_MIBSPI1NCS_3 ) \
  per_pin_action( common_arg, TMS570_BALL_N19_AD1EVT ) \
  per_pin_action( common_arg, TMS570_BALL_N15_EMIF_DATA_3 ) \
  per_pin_action( common_arg, TMS570_BALL_N17_EMIF_nCS_0 ) \
  per_pin_action( common_arg, TMS570_BALL_M15_EMIF_DATA_2 ) \
  per_pin_action( common_arg, TMS570_BALL_K17_EMIF_nCS_3 ) \
  per_pin_action( common_arg, TMS570_BALL_M17_EMIF_nCS_4 ) \
  per_pin_action( common_arg, TMS570_BALL_L15_EMIF_DATA_1 ) \
  per_pin_action( common_arg, TMS570_BALL_P1_HET1_24 ) \
  per_pin_action( common_arg, TMS570_BALL_A14_HET1_26 ) \
  per_pin_action( common_arg, TMS570_BALL_K15_EMIF_DATA_0 ) \
  per_pin_action( common_arg, TMS570_BALL_G19_MIBSPI1NENA ) \
  per_pin_action( common_arg, TMS570_BALL_H18_MIBSPI5NENA ) \
  per_pin_action( common_arg, TMS570_BALL_J18_MIBSPI5SOMI_0 ) \
  per_pin_action( common_arg, TMS570_BALL_J19_MIBSPI5SIMO_0 ) \
  per_pin_action( common_arg, TMS570_BALL_H19_MIBSPI5CLK ) \
  per_pin_action( common_arg, TMS570_BALL_R2_MIBSPI1NCS_0 ) \
  per_pin_action( common_arg, TMS570_BALL_E18_HET1_08 ) \
  per_pin_action( common_arg, TMS570_BALL_K19_HET1_28 ) \
  per_pin_action( common_arg, TMS570_BALL_D17_EMIF_nWE ) \
  per_pin_action( common_arg, TMS570_BALL_D16_EMIF_BA_1 ) \
  per_pin_action( common_arg, TMS570_BALL_C17_EMIF_ADDR_21 ) \
  per_pin_action( common_arg, TMS570_BALL_C16_EMIF_ADDR_20 ) \
  per_pin_action( common_arg, TMS570_BALL_C15_EMIF_ADDR_19 ) \
  per_pin_action( common_arg, TMS570_BALL_D15_EMIF_ADDR_18 ) \
  per_pin_action( common_arg, TMS570_BALL_E13_EMIF_BA_0 ) \
  per_pin_action( common_arg, TMS570_BALL_C14_EMIF_ADDR_17 ) \
  per_pin_action( common_arg, TMS570_BALL_D14_EMIF_ADDR_16 ) \
  per_pin_action( common_arg, TMS570_BALL_E12_EMIF_nOE ) \
  per_pin_action( common_arg, TMS570_BALL_D19_HET1_10 ) \
  per_pin_action( common_arg, TMS570_BALL_E11_EMIF_nDQM_1 ) \
  per_pin_action( common_arg, TMS570_BALL_B4_HET1_12 ) \
  per_pin_action( common_arg, TMS570_BALL_E9_EMIF_ADDR_5 ) \
  per_pin_action( common_arg, TMS570_BALL_C13_EMIF_ADDR_15 ) \
  per_pin_action( common_arg, TMS570_BALL_A11_HET1_14 ) \
  per_pin_action( common_arg, TMS570_BALL_C12_EMIF_ADDR_14 ) \
  per_pin_action( common_arg, TMS570_BALL_M2_GIOB_0 ) \
  per_pin_action( common_arg, TMS570_BALL_E8_EMIF_ADDR_4 ) \
  per_pin_action( common_arg, TMS570_BALL_B11_HET1_30 ) \
  per_pin_action( common_arg, TMS570_BALL_E10_EMIF_nDQM_0 ) \
  per_pin_action( common_arg, TMS570_BALL_E7_EMIF_ADDR_3 ) \
  per_pin_action( common_arg, TMS570_BALL_C11_EMIF_ADDR_13 ) \
  per_pin_action( common_arg, TMS570_BALL_C10_EMIF_ADDR_12 ) \
  per_pin_action( common_arg, TMS570_BALL_F3_MIBSPI1NCS_1 ) \
  per_pin_action( common_arg, TMS570_BALL_C9_EMIF_ADDR_11 ) \
  per_pin_action( common_arg, TMS570_BALL_D5_EMIF_ADDR_1 ) \
  per_pin_action( common_arg, TMS570_BALL_K2_GIOB_1 ) \
  per_pin_action( common_arg, TMS570_BALL_C8_EMIF_ADDR_10 ) \
  per_pin_action( common_arg, TMS570_BALL_C7_EMIF_ADDR_9 ) \
  per_pin_action( common_arg, TMS570_BALL_D4_EMIF_ADDR_0 ) \
  per_pin_action( common_arg, TMS570_BALL_C5_EMIF_ADDR_7 ) \
  per_pin_action( common_arg, TMS570_BALL_C4_EMIF_ADDR_6 ) \
  per_pin_action( common_arg, TMS570_BALL_E6_EMIF_ADDR_2 ) \
  per_pin_action( common_arg, TMS570_BALL_C6_EMIF_ADDR_8 ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_SPI4CLK ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_SPI4SIMO ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_SPI4SOMI ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_SPI4NENA ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_SPI4NCS_0 ) \
  per_pin_action( common_arg, TMS570_BALL_A13_HET1_17 ) \
  per_pin_action( common_arg, TMS570_BALL_B13_HET1_19 ) \
  per_pin_action( common_arg, TMS570_BALL_H4_HET1_21 ) \
  per_pin_action( common_arg, TMS570_BALL_J4_HET1_23 ) \
  per_pin_action( common_arg, TMS570_BALL_M3_HET1_25 ) \
  per_pin_action( common_arg, TMS570_BALL_A9_HET1_27 ) \
  per_pin_action( common_arg, TMS570_BALL_A3_HET1_29 ) \
  per_pin_action( common_arg, TMS570_BALL_J17_HET1_31 ) \
  per_pin_action( common_arg, TMS570_BALL_W6_MIBSPI5NCS_2 ) \
  per_pin_action( common_arg, TMS570_BALL_T12_MIBSPI5NCS_3 ) \
  per_pin_action( common_arg, TMS570_BALL_E19_MIBSPI5NCS_0 ) \
  per_pin_action( common_arg, TMS570_BALL_B6_MIBSPI5NCS_1 ) \
  per_pin_action( common_arg, TMS570_BALL_E16_MIBSPI5SIMO_1 ) \
  per_pin_action( common_arg, TMS570_BALL_H17_MIBSPI5SIMO_2 ) \
  per_pin_action( common_arg, TMS570_BALL_G17_MIBSPI5SIMO_3 ) \
  per_pin_action( common_arg, TMS570_BALL_E17_MIBSPI5SOMI_1 ) \
  per_pin_action( common_arg, TMS570_BALL_H16_MIBSPI5SOMI_2 ) \
  per_pin_action( common_arg, TMS570_BALL_G16_MIBSPI5SOMI_3 ) \
  per_pin_action( common_arg, TMS570_BALL_D3_SPI2NENA ) \
  per_pin_action( common_arg, \
  TMS570_MMR_SELECT_EMIF_CLK_SEL | TMS570_PIN_CLEAR_RQ_MASK ) \
  per_pin_action( common_arg, \
  TMS570_BALL_F2_GIOB_2 | TMS570_PIN_CLEAR_RQ_MASK ) \
  per_pin_action( common_arg, \
  TMS570_MMR_SELECT_MII_MODE | TMS570_PIN_CLEAR_RQ_MASK ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_ADC_TRG1 )

/*
 * The next construct allows to compute values for individual
 * PINMMR registers based on the multiple processing
 * complete pin functions list at compile time.
 * Each line computes 32-bit value which selects function
 * of consecutive four pins. Each pin function is defined
 * by single byte.
 */
static const uint32_t tms570_pinmmr_init_data[] = {
  TMS570_PINMMR_REG_VAL( 0, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 1, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 2, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 3, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 4, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 5, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 6, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 7, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 8, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 9, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 10, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 11, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 12, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 13, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 14, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 15, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 16, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 17, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 18, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 19, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 20, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 21, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 22, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 23, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 24, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 25, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 26, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 27, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 28, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 29, TMS570_PINMMR_INIT_LIST ),
  TMS570_PINMMR_REG_VAL( 30, TMS570_PINMMR_INIT_LIST ),
};

/**
 * @brief setups pin multiplexer according to precomputed registers values (HCG:muxInit)
 */
void tms570_pinmux_init( void )
{
  tms570_bsp_pinmmr_config( tms570_pinmmr_init_data, 0,
    RTEMS_ARRAY_SIZE( tms570_pinmmr_init_data ) );
}

void tms570_emif_sdram_init(void)
{
  uint32_t dummy;
  uint32_t sdtimr = 0;
  uint32_t sdcr = 0;

  /* Do not run attempt to initialize SDRAM when code is running from it */
  if ( tms570_running_from_sdram() )
    return;

  sdtimr = TMS570_EMIF_SDTIMR_T_RFC_SET( sdtimr, 6 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RP_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RCD_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_WR_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RAS_SET( sdtimr, 4 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RC_SET( sdtimr, 6 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RRD_SET( sdtimr, 2 - 1 );

  TMS570_EMIF.SDTIMR = sdtimr;

  /* Minimum number of ECLKOUT cycles from Self-Refresh exit to any command */
  TMS570_EMIF.SDSRETR = 5;
  /* Define the SDRAM refresh period in terms of EMIF_CLK cycles. */
  TMS570_EMIF.SDRCR = 2000;

  /* SR - Self-Refresh mode bit. */
  sdcr |= TMS570_EMIF_SDCR_SR * 0;
  /* field: PD - Power Down bit controls entering and exiting of the power-down mode. */
  sdcr |= TMS570_EMIF_SDCR_PD * 0;
  /* PDWR - Perform refreshes during power down. */
  sdcr |= TMS570_EMIF_SDCR_PDWR * 0;
  /* NM - Narrow mode bit defines whether SDRAM is 16- or 32-bit-wide */
  sdcr |= TMS570_EMIF_SDCR_NM * 1;
  /* CL - CAS Latency. */
  sdcr = TMS570_EMIF_SDCR_CL_SET( sdcr, 2 );
  /* CL can only be written if BIT11_9LOCK is simultaneously written with a 1. */
  sdcr |= TMS570_EMIF_SDCR_BIT11_9LOCK * 1;
  /* IBANK - Internal SDRAM Bank size. */
  sdcr = TMS570_EMIF_SDCR_IBANK_SET( sdcr, 2 ); /* 4-banks device */
  /* Page Size. This field defines the internal page size of connected SDRAM devices. */
  sdcr = TMS570_EMIF_SDCR_PAGESIZE_SET( sdcr, 0 ); /* elements_256 */

  TMS570_EMIF.SDCR = sdcr;

  dummy = *(volatile uint32_t*)TMS570_MEMORY_SDRAM_ORIGIN;
  (void) dummy;
  TMS570_EMIF.SDRCR = 31;

  /* Define the SDRAM refresh period in terms of EMIF_CLK cycles. */
  TMS570_EMIF.SDRCR = 312;
}

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
