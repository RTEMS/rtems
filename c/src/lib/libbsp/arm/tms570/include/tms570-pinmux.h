/**
 * @file tms570-pinmux.h
 *
 * @ingroup tms570
 *
 * @brief I/O Multiplexing Module (IOMM) basic support
 */

/*
 * Copyright (c) 2015 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_PINMUX_H
#define LIBBSP_ARM_TMS570_PINMUX_H

#ifndef ASM
#include <bsp/tms570.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define TMS570_PIN_NUM_SHIFT 0
#define TMS570_PIN_NUM_MASK  0x0fff

#define TMS570_PIN_FNC_SHIFT 12
#define TMS570_PIN_FNC_MASK  0xf000

#define TMS570_PIN_FNC_AUTO  (-1)

#define TMS570_PIN_AND_FNC(pin, fnc) \
  ((pin) | ((fnc)<<TMS570_PIN_FNC_SHIFT))

#define TMS570_BALL_WITH_MMR(mmrx, pos) \
  ((pos) | ((mmrx)<<2))

/* Definition for pins/balls which has to be set for Ethernet MII */

#define TMS570_BALL_C3  TMS570_BALL_WITH_MMR(0, 2)
#define TMS570_BALL_C3_MIBSPI3NCS_3 TMS570_PIN_AND_FNC(TMS570_BALL_C3, 0)
#define TMS570_BALL_C3_I2C_SCL TMS570_PIN_AND_FNC(TMS570_BALL_C3, 1)

#define TMS570_BALL_F3 TMS570_BALL_WITH_MMR(20, 2)
#define TMS570_BALL_F3_MII_COL TMS570_PIN_AND_FNC(TMS570_BALL_F3, 2)

#define TMS570_BALL_B4 TMS570_BALL_WITH_MMR(17, 2)
#define TMS570_BALL_B4_MII_CRS TMS570_PIN_AND_FNC(TMS570_BALL_B4, 1)

#define TMS570_BALL_B11 TMS570_BALL_WITH_MMR(19, 1)
#define TMS570_BALL_B11_MII_RX_DV TMS570_PIN_AND_FNC(TMS570_BALL_B11, 1)

#define TMS570_BALL_N19 TMS570_BALL_WITH_MMR(10, 0)
#define TMS570_BALL_N19_MII_RX_ER TMS570_PIN_AND_FNC(TMS570_BALL_N19, 1)

#define TMS570_BALL_K19 TMS570_BALL_WITH_MMR(14, 1)
#define TMS570_BALL_K19_MII_RX_CLK TMS570_PIN_AND_FNC(TMS570_BALL_K19, 1)

#define TMS570_BALL_P1 TMS570_BALL_WITH_MMR(11, 3)
#define TMS570_BALL_P1_MII_RXD_0 TMS570_PIN_AND_FNC(TMS570_BALL_P1, 2)

#define TMS570_BALL_A14 TMS570_BALL_WITH_MMR(12, 0)
#define TMS570_BALL_A14_MII_RXD_1 TMS570_PIN_AND_FNC(TMS570_BALL_A14, 1)

#define TMS570_BALL_G19 TMS570_BALL_WITH_MMR(12, 2)
#define TMS570_BALL_G19_MII_RXD_2 TMS570_PIN_AND_FNC(TMS570_BALL_G19, 2)

#define TMS570_BALL_H18 TMS570_BALL_WITH_MMR(12, 3)
#define TMS570_BALL_H18_MII_RXD_3 TMS570_PIN_AND_FNC(TMS570_BALL_H18, 2)

#define TMS570_BALL_D19 TMS570_BALL_WITH_MMR(17, 0)
#define TMS570_BALL_D19_MII_TX_CLK TMS570_PIN_AND_FNC(TMS570_BALL_D19, 1)

#define TMS570_BALL_J18 TMS570_BALL_WITH_MMR(13, 0)
#define TMS570_BALL_J18_MII_TXD_0 TMS570_PIN_AND_FNC(TMS570_BALL_J18, 2)

#define TMS570_BALL_J19 TMS570_BALL_WITH_MMR(13, 1)
#define TMS570_BALL_J19_MII_TXD_1 TMS570_PIN_AND_FNC(TMS570_BALL_J19, 2)

#define TMS570_BALL_R2 TMS570_BALL_WITH_MMR(13, 3)
#define TMS570_BALL_R2_MII_TXD_2 TMS570_PIN_AND_FNC(TMS570_BALL_R2, 2)

#define TMS570_BALL_E18 TMS570_BALL_WITH_MMR(14, 0)
#define TMS570_BALL_E18_MII_TXD_3 TMS570_PIN_AND_FNC(TMS570_BALL_E18, 2)

#define TMS570_BALL_H19 TMS570_BALL_WITH_MMR(13, 2)
#define TMS570_BALL_H19_MII_TXEN TMS570_PIN_AND_FNC(TMS570_BALL_H19, 2)

#define TMS570_BALL_V5 TMS570_BALL_WITH_MMR(7, 1)
#define TMS570_BALL_V5_MDCLK TMS570_PIN_AND_FNC(TMS570_BALL_V5, 2)

#define TMS570_BALL_G3 TMS570_BALL_WITH_MMR(8, 1)
#define TMS570_BALL_G3_MDIO TMS570_PIN_AND_FNC(TMS570_BALL_G3, 2)

#define TMS570_BALL_XX TMS570_BALL_WITH_MMR(29, 3)
#define TMS570_BALL_XX_GMII_SEL TMS570_PIN_AND_FNC(TMS570_BALL_XX, 0)

/* Generic functions select pin to peripheral connection */

void tms570_bsp_pin_set_function(int pin_num, int pin_fnc);

void tms570_bsp_pin_clear_function(int pin_num, int pin_fnc);

static inline void
tms570_bsp_pin_to_pinmmrx(volatile uint32_t **pinmmrx, unsigned int *pin_shift,
                          int pin_num)
{
  pin_num = (pin_num & TMS570_PIN_NUM_MASK) >> TMS570_PIN_NUM_SHIFT;
  *pinmmrx = &TMS570_IOMM.PINMUX.PINMMR0 + (pin_num >> 2);
  *pin_shift = (pin_num & 0x3)*8;
}

#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */
