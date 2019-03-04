/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief Initialize pin multiplexers.
 */
/*
 * Copyright (c) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>
#include <bsp/tms570.h>
#include <bsp/tms570-pinmux.h>
#include <rtems.h>
#include <bsp/tms570_hwinit.h>

/*
 * To check that content is right generated use
 *
 * objdump --section=.rodata -s init_pinmux.o
 */
#if 0

/*
 * Test of use of the default pins configuration with one line added.
 * This can be used to concatenate partial lists but care has to
 * be taken to not attempt to override already defined pin.
 * This would not work and result in two PINMMR bits set
 * for given pine.
 */

#ifndef TMS570_PINMMR_INIT_LIST
  #define TMS570_PINMMR_INIT_LIST(per_pin_action, common_arg) \
     TMS570_PINMMR_DEFAULT_INIT_LIST(per_pin_action, common_arg) \
     per_pin_action(common_arg, TMS570_BALL_E3_HET2_18)
#endif

#else

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
  TMS570_MMR_SELECT_GMII_SEL | TMS570_PIN_CLEAR_RQ_MASK ) \
  per_pin_action( common_arg, TMS570_MMR_SELECT_ADC_TRG1 ) \


#endif

/*
 * The next construct allows to compute values for individual
 * PINMMR registers based on the multiple processing
 * complete pin functions list at compile time.
 * Each line computes 32-bit value which selects function
 * of consecutive four pins. Each pin function is defined
 * by single byte.
 */
const uint32_t tms570_pinmmr_init_data[] = {
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

#if 0

/*
 * Alternative option how to set function of individual pins
 * or use list for one by one setting. This is much slower
 * and consumes more memory to hold complete list.
 *
 * On the other hand this solution can be used for configuration
 * or reconfiguration of some shorter groups of pins at runtime.
 *
 */

const uint32_t tms570_pinmmr_init_list[] = {
  TMS570_PINMMR_COMA_LIST( TMS570_PINMMR_INIT_LIST )
};

void tms570_pinmux_init_by_list( void )
{
  int pincnt = RTEMS_ARRAY_SIZE( tms570_pinmmr_init_list );
  const uint32_t *pinfnc = tms570_pinmmr_init_list;

  while ( pincnt-- )
    tms570_bsp_pin_config_one( *(pinfnc++) );
}
#endif
